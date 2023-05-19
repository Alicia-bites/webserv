#include "Cgi.hpp"

using namespace webserv;
using namespace webserv::io;
using namespace webserv::utils;

Cgi::Cgi( webserv::http::Response &response, const std::string &cgiCmd, const std::string &contentPath):
	_metaVariables(MetaVariable(response.request)),
	_contentPath(contentPath),
	_executed(false),
	_outputRead(false),
	_childPid(0),
	_response(response),
	_cgiCmd(_path(cgiCmd))
	
{
	_argv = (char **)malloc(sizeof(char *) * 3);
	if (_argv == NULL)
		throw exceptions::cgi::AllocationError();
	_argv[0] = (char *)_cgiCmd.c_str();
	_argv[1] = (char *)_contentPath.c_str();
	_argv[2] = NULL;
	_argsAllocated = true;
	_body.clear();
	_headerRead = false;
}

// DESTRUCTOR
Cgi::~Cgi( void )
{
	// If we come to the CGI destructor, we've read everything we need to read, so we can kill the child process
	if (_executed == true)
	{
		kill(_childPid, SIGTERM);
		waitpid(_childPid, NULL, 0);
		close(this->_pfdOut[0]);
	}
	if (_argsAllocated == true)
		free(_argv);
}

// METHOD

/**
 * @brief Write request body to the pipe file descriptor which is replacing the STDIN
 * 
 * @todo Fix the issue when the BODY_BUFFER_SIZE seems small
*/
void	Cgi::_forwardPostBody( void )
{
	if (this->_response.request.method != "POST")
		return ;
	size_t readSize = 0;
    char buffer[BODY_BUFFER_SIZE];
	buffer[0] = '\0';
	readSize = this->_response.request.readBody(buffer, BODY_BUFFER_SIZE);
	buffer[readSize] = '\0';
	while (readSize > 0)
	{
		if (write(this->_pfdIn[1], buffer, readSize) <= 0)
		{
			webserv::logger::error(" Error to write into pfd");
			if (this->_pfdIn[0] > 0)
			{
				close(this->_pfdIn[0]);
				this->_pfdIn[0] = -1;
			}
			if (this->_pfdIn[1] > 0)
			{
				close(this->_pfdIn[1]);
				this->_pfdIn[1] = -1;
			}
			if (this->_pfdOut[1] > 0)
			{
				close(this->_pfdOut[1]);
				this->_pfdOut[1] = -1;
			}
			throw exceptions::cgi::WriteIntoFdFailed();
		}
		readSize = this->_response.request.readBody(buffer, BODY_BUFFER_SIZE);
		buffer[readSize] = '\0';
	}
}

/**
 * @brief Setup the pipe file descriptors
 * 
 * @throws exceptions::cgi::PipeFailed if pipe() failed
*/
void	Cgi::_createPipeFd( void )
{
	if (pipe(this->_pfdIn) != 0)
		throw exceptions::cgi::PipeFailed();
	if (pipe(this->_pfdOut) != 0)
	{
		close(this->_pfdIn[0]);
		close(this->_pfdIn[1]);
		this->_pfdIn[0] = -1;
		this->_pfdIn[1] = -1;
		throw exceptions::cgi::PipeFailed();
	}
}

/**
 * @brief Checks whether the given path is an absolute path or not, and if it is not, it will be converted to an absolute path
 * 
 * @param candidate The path to check
 * @return std::string The absolute path of the given path (with correct permissions)
 * 
 * @throws exceptions::cgi::NoSuchFile If no viable path was found
 */
std::string	Cgi::_path( const std::string &candidate )
{
	if ((is(AccessFlags::FILE, candidate) && is(AccessFlags::EXECUTABLE, candidate)) == true)
		return candidate;
	std::string	tmpPath = getenv("PATH");
	std::vector<std::string> allPath = split(tmpPath, ':');
	for (size_t i = 0; i < allPath.size(); i++)
	{
		std::string execName = path_join(allPath[i], candidate);
		if (is(AccessFlags::FILE, execName) == false && is(AccessFlags::EXECUTABLE, execName) == false)
			continue ;
		return (execName);
	}
	throw exceptions::cgi::NoSuchFile();
}

/**
 * @brief Execute the CGI script
 * 
 * @param envp 
 * @return int 
 */
int	Cgi::_exec()
{
	pid_t	pid;

	this->_executed = true;
	pid = fork();
	if (pid < 0)
		throw exceptions::cgi::ForkFailed();
	if (pid == 0)
	{
		close(this->_pfdIn[1]);
		dup2(this->_pfdIn[0], STDIN_FILENO);
		dup2(this->_pfdOut[1], STDOUT_FILENO);
		this->_pfdIn[1] = -1;
		// close(this->_pfdIn[0]);
		// close(this->_pfdOut[0]);
		// close(this->_pfdOut[1]);
		// DEBUG TO LIST ENVP
		// char **test = this->_metaVariables.getVars();
		// int i = 0;
		// while (test[i] != NULL)
		// {
		// 	std::cout << CYAN1 << "[TEST POST CGI] env: " << test[i] << RESET << std::endl;
		// 	i++;
		// }
		execve(this->_cgiCmd.c_str(), this->_argv, this->_metaVariables.getVars());
		kill(pid, SIGKILL);
		return 0;
	}
	else
	{
		close(this->_pfdIn[0]);
		_forwardPostBody();
		close(this->_pfdIn[1]);
		close(this->_pfdOut[1]);
		this->_pfdIn[0] = -1;
		this->_pfdIn[1] = -1;
		this->_pfdOut[1] = -1;
		_childPid = pid;
		return (this->_pfdOut[0]);
	}
}

// https://www.gnu.org/software/libc/manual/html_mono/libc.html#Temporary-Files
// https://www.gnu.org/software/libc/manual/html_mono/libc.html#File-Positioning
int	Cgi::_run()
{
	this->_createPipeFd();
	return this->_exec();
}

/**
 * @brief Reads n bytes from the CGI's stdout and writes them to the given buffer
 * 
 * 
 * @param dest Destination buffer, must be at least n bytes long
 * @param n Number of bytes to read
 * @return size_t The number of bytes read, throws an exception if the socket is closed
 * 
 * @note Automatically executes the CGI script if it has not been executed yet
 * @note Closes all file descriptors and frees all allocated memory at destruction
 * @note Automatically fills the `headers` attribute with the CGI's headers
 * 
 * @throws exceptions::io::PipeClosed if the CGI's stdout was closed
*/
ssize_t Cgi::readBody(char *dest, size_t n)
{
	ssize_t readSize = 0;

	// If we've already read the body, return 0 (EOF)
	if (_outputRead == true)
		return 0;
	
	// Execute the CGI script if it has not been executed yet (creates pipeline)
	if (!_executed)
		this->_run();

	// Wait for input data using poll
	struct pollfd fds[1];
	fds[0].fd = this->_pfdOut[0];
	fds[0].events = POLLIN;
	int ret = poll(fds, 1, 2000);
	if (ret == -1)
		throw exceptions::cgi::ChildTimeout(); // process most likely died
	if (ret == 0)
		throw exceptions::cgi::ChildTimeout(); // we took more than 2000ms to read data from CGI, too long, kill it

	// AT THE FIRST ITERATION OF THE FUNCTION, PARSE HEADER UNTIL TO HAVE
	// FULL HEADER AND A PART OF THE BODY
	// SET _headerRead = true TO AVOID ANY ADDITIONAL READING ON NEXT
	// ITERATION OF THE FUNCTION
	// NOTE: HEADER END WHEN CRLFCRLF
	if (_headerRead == false)
	{
		this->parseHeader(dest, n, readSize);
		_headerRead = true;
	}
	// SEPARATE CONDITIONAL 'IF' TO MAKE SURE TO TRANSFERT A MINIMUM OF BODY
	// BY FORWARDING THE REST OF BODY AFTER PARSING HEADER
	// OR BY READING PFDOUT
	if (this->_body.empty() == true)
	{
		readSize = read(this->_pfdOut[0], dest, n);
		dest[readSize] = '\0';
	}
	else
	{
		this->forwardBody(dest, n, readSize);
	}
	if (readSize == 0)
		_outputRead = true;
	return readSize;
}

void	Cgi::parseHeader( char *dest, size_t n, ssize_t &readSize )
{
	readSize = read(this->_pfdOut[0], dest, n);
	dest[readSize] = '\0';
	std::string	tmpStr = std::string(dest); // CAN CONTAIN THE HEADER AND A PART OF THE BODY
	size_t		npos = tmpStr.find(HEADER_END);
	while (readSize > 0 && npos == std::string::npos) // LOOP UNTIL HEADER IS FULLY PRESENT IN THE STRING
	{
		memset(dest, '\0', n);
		readSize = read(this->_pfdOut[0], dest, n);
		dest[readSize] = '\0';
		tmpStr.append(dest);
		npos = tmpStr.find(HEADER_END);
	}
	if (npos == std::string::npos) // THROW EXEPTIONS IF NO HEADER END
		throw exceptions::http::InvalidHeaderFormat();
	else
	{
		this->_body = tmpStr.substr(npos + 4); // SAVE THE BODY PART
		std::string	tmpHeader = tmpStr.substr(0, npos); // COPY ONLY THE HEADER PART INTO A NEW STRING
		std::istringstream stream(tmpHeader);
		std::string tmpLine;
		while (std::getline(stream, tmpLine)) // LOOP THROUGH EACH LINE OF THE HEADER
		{
			size_t tmpPos = tmpLine.rfind("\r");
			if (tmpPos != std::string::npos)
				tmpLine.erase(tmpPos);
			tmpPos = tmpLine.find(HEADER_LINE_SEP);
			parseHeaderLine(tmpLine, headers);
		}
	}
}

void	Cgi::forwardBody(char *dest, size_t n, ssize_t &readSize)
{
	memset(dest, '\0', n); // SET DATA TO \0 CHAR TO EMPTY dest
	readSize = this->_body.size(); // ENSURE THE CORRECT RETURN VALUE
	if ((size_t)readSize > n) // PROTECTION OF OVERFLOW
		readSize = n;
	memcpy(dest, this->_body.c_str(), readSize); // AND COPY CONTENT OF BODY TO BE USED BY Request LOOP
	this->_body.erase(0, readSize); // DELETE COPIED DATA TO AVOID ANY DUPLICATE OF DATA FOR THE NEXT ITERATION OF THE FUNCTION
	// std::cout << "[TEST POST CGI] dest:\n" << dest << std::endl;
}

bool Cgi::eof(void) {
	return _outputRead;
}

void	Cgi::showMetaVariables( void )
{
	std::cout << "MetaVariables: " <<  this->_metaVariables << std::endl;
}
