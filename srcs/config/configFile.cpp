#include "configFile.hpp"
#include <stdexcept>

namespace webserv
{
	namespace config
	{
		// default constructor
		ConfigFile::ConfigFile()
		: fileName_("default")
		, lines_(std::vector<std::string>())
		, errors_(std::vector<std::string>())
		, servers(ConfigFile::map_type())
		{}

		// constructor
		ConfigFile::ConfigFile(const std::string & fileName)
		: fileName_(fileName.empty() ? "default" : fileName)
		, lines_(std::vector<std::string>())
		, errors_(std::vector<std::string>())
		, servers(ConfigFile::map_type())
		{
			if (fileName_ == "")
				throw webserv::exceptions::config::InvalidFileName();

			//WARNING : exception thrower
			isValid();

			//copy content of file in vector lines_
			readFile();

			// parse file
			int err = parseFile();
			produceConfigLogs();
			if (err == 0)
				throw webserv::exceptions::config::ConfigError();
		}

		// copy constructor
		ConfigFile::ConfigFile(const ConfigFile & src)
		: fileName_(src.fileName_)
		, lines_(src.lines_)
		, errors_(src.errors_)
		, servers(src.servers)
		{}

		//destructor
		ConfigFile::~ConfigFile()
		{}

		ConfigFile &	ConfigFile::operator=(const ConfigFile & src)
		{
			if (this != &src)
			{
				fileName_ = src.fileName_;
				lines_ = src.lines_;
				errors_ = src.errors_;
				servers = src.servers;
			}

			return *this;
		}

		std::vector<std::string>	ConfigFile::getErrors() const
		{
			return errors_;
		}

		// ConfigFile::map_type::iterator const	ConfigFile::samePortAs(std::string name, int port, std::string IP) {
			// std::cout << "name = " << name << std::endl;
			// for (map_type::iterator it = servers.begin(); it != servers.end(); it++) {
				// if (port == it->second.getPort() && IP == it->second.getIP()) {
					// std::cout << "returning this server : " << it->second.getServerName() << std::endl;
					// return it;
				// }
			// }
			// return servers.end();
		// }

		Server const & ConfigFile::getServer(std::string const & host, int & port) {
			
			// first, check if hots correspond to a server_name in configfile
			// if so, return it.
			map_type::iterator it = servers.find(host);
			if (it != servers.end())
				return it->second;

			// if not, check for IP and port. If the exact same ones are found in a server block,
			// return it.
			for (map_type::iterator it = servers.begin(); it != servers.end(); it++) {
				if (port == it->second.getPort())
					return it->second;
			}

			// FOR DEBUG
			std::cerr << LIGHTSLATEBLUE << "No server found, so I give the first on the list." << RESET << std::endl;
			
			// if no server block with corresponding server_name, 
			// return first server in the map.
			return servers.begin()->second;

		}

		// Server const & ConfigFile::getServer(std::string const & host)
		// {
		// 	map_type::iterator it = servers.find(host);
		// 	if (it != servers.end()) {
		// 			return it->second;
		// 	}

		// 	// FOR DEBUG
		// 	std::cerr << LIGHTSLATEBLUE << "No server found, so I give the first on the list." << RESET << std::endl;
			
		// 	// if no server block with corresponding server_name, 
		// 	// return first server in the map.
		// 	return servers.begin()->second;
		// }

		Location const  ConfigFile::getLocation(std::string const & serverName, std::string const & filePath)
		{
			map_type::iterator it = servers.find(serverName);
			if (it != servers.end())
			{
				Server::map_type const locations = it->second.getLocations();
				Server::map_type::const_iterator ite = locations.find(filePath);
				if (ite != locations.end())
					return ite->second;
				else
					return it->second.getDefaultLocation();
			}
			throw std::runtime_error("No server found.");
		}

		void    ConfigFile::initReqServer()
		{
			counterServer_["server"] = 0;
			counterServer_["server_name"] = 0;
			counterServer_["root"] = 0;
			counterServer_["listen"] = 0;
			counterServer_["max_body_size"] = 0;
			counterServer_["location"] = 0;
			counterServer_["index"] = 0;
			counterServer_["cgi"] = 0;
			counterServer_["directory_index"] = 0;
			counterServer_["redirect"] = 0;
			counterServer_["error_page"] = 0;
			counterServer_["upload_dir"] = 0;
		}

		void    ConfigFile::initReqLocation()
		{
			counterLocation_["location"] = 0;
			counterLocation_["max_body_size"] = 0;
			counterLocation_["index"] = 0;
			counterLocation_["directory_index"] = 0;
			counterLocation_["upload_dir"] = 0;
			counterLocation_["root"] = 0;
			counterLocation_["allow_upload"] = 0;

		}


		void	ConfigFile::isValid()
		{
			if (input_.is_open())
				input_.close();

			input_.open(fileName_.c_str());
			if (input_.peek() == EOF)
				throw webserv::exceptions::config::EmptyFile();
		}

		// trim inline comments
		void	ConfigFile::trimInlineComment(std::string & line)
		{
			size_t pos = line.find_first_of("#");
			if (pos != std::string::npos)
				line = line.substr(0, pos);
		}

		// copy file content in vector lines_ (class private attribute)
		void	ConfigFile::readFile()
		{
			std::string line;
			while (std::getline(input_, line))
			{
				trimInlineComment(line);
				lines_.push_back(line);
			}
		}

		// return first word of a line
		std::string    ConfigFile::firstWord(std::string & line)
		{
			std::istringstream iss(line); // create an input string stream from the line

			std::string firstWord;
			iss >> firstWord; // extract the first word from the stream
			
			return firstWord;
		}
		
		// parse server_name property
		void	ConfigFile::parseServerName(std::string & line, std::vector<std::string> & server_name)
		{
			if (firstWord(line) == "server_name")
			{
				counterServer_["server_name"]++;

				if (line == "server_name")
					errors_.push_back("Error: no server_name found. Check line : " + line);

				size_t found = line.find("server_name ");
				if (found != std::string::npos)
				{
					std::string allServerNames = line.substr(found +  11);
					// split allServerNames on every space
					std::stringstream ss(allServerNames);
					std::string name;
					while (ss >> name)
						server_name.push_back(name);
				}
				else
					errors_.push_back("Error : wrong syntax. Expected : server_name <server's names>");
				
				if (hasDuplicates<std::string>(server_name))
					errors_.push_back("Error : cannot have twice the same server name");

				// FOR DEBUG
				// for (size_t i = 0; i < server_name.size(); i++) {
					// std::cout << server_name[i] << std::endl;
				// }
			}
		}

		// uses inet_pton to convert IP address to IPV4.
		// If it failes, it means the IP address was not valid
		// in the first place.
		bool ConfigFile::isValidIpAddress(const char *ipAddress)
		{
			struct in_addr addr;
			int res = inet_pton(AF_INET, ipAddress, &addr);
			return res == 1;
		}

		bool ConfigFile::isLastWord(const std::string& line, const std::string& word)
		{
			size_t pos = line.rfind(word);
			if (pos == std::string::npos)
				return false;

			size_t end_pos = pos + word.length();
			if (end_pos == line.length())
				return true;
			else
				// word is not the last word in the line
				return false;
		}

		bool    ConfigFile::isNumAndDots(const std::string & line)
		{
			for (size_t i = 0; i < line.length(); i++)
			{
				if (isalpha(line[i]))
					return 0;
			}
			return 1;
		}

		// reduce number of spaces to 1 if there are more than one space in line
		void	ConfigFile::trimSpaces(std::string & line)
		{
			size_t pos = 0;
			while ((pos = line.find("  ", pos)) != std::string::npos)
				line.erase(pos, 1);
		}

		// replace tabs by spaces
		void	ConfigFile::replaceTabs(std::string & line)
		{
			// set pos to skip the first tabs that I want to keep
			// because I use it as an indent indicator.
			size_t pos = line.find_first_not_of('\t');
			// replace every tab with space.
			while ((pos = line.find('\t', pos)) != std::string::npos)
				line.replace(pos, 1, " ");
		}

		// returns true if string is interely made of digits, false if
		// at least one non-digit char is found.
		bool	stringIsDigit(std::string line)
		{
			for (size_t i = 0; i < line.length(); i++)
			{
				if (!isdigit(line[i]))
					return 0;
			}
			return 1;
		}

		// parse listen property
		std::pair<std::string, int>    ConfigFile::parseListen(std::string & line)
		{
			std::string IP = "";
			int port = 0;

			if (firstWord(line) == "listen")
			{
				counterServer_["listen"]++;
				
				if (line == "listen")
					errors_.push_back("Error: no ip address found. Check line : " + line);

				// extract port
				std::size_t n = line.find(":");
				if (n != std::string::npos)
				{
					std::string endLine = line.substr(n + 1);
					// check if overflow
					if (!stringIsDigit(endLine))
						errors_.push_back("Error: Invalid port. Value should be between 1 and 65535. Check line " + line);
					port = atoi(endLine.c_str());
					line.erase(n, line.size() - n);

					// if port was found
					if (port < 1 || port > 65535)
						errors_.push_back("Error: Invalid port. Value should be between 1 and 65535. Check line : " + line);
				}
				

				// std::cout << "port = " << port << std::endl;
				// check IP address
				std::size_t found = line.find("listen "); // this does not work if there is more than 1 space
				if (found != std::string::npos)
				{
					std::string ipAddress = line.substr(found + 7);
					if (!isNumAndDots(ipAddress))
						return (std::make_pair("", 0));
					if (isValidIpAddress(ipAddress.c_str()) == 0)
						errors_.push_back("Error: Invalid IP address (" + ipAddress + "). Check line : " + line);
					
					IP = ipAddress;
				}
			}
			return (std::make_pair(IP, port));
		}

		// format accepted : 
		// 10B, 10b, 200K, 200k, 600m, 600M, 1g, 1G
		bool	ConfigFile::isValidSize(const std::string & size)
		{
			int length = size.length();

			if (length < 2)
				return false;

			if (!isdigit(*(size.begin())))
				return false;

			char lastChar = size[length - 1];
			if (!isalpha(lastChar))
				return false;

			char secondLastChar = size[length - 2];
			if (!isdigit(secondLastChar))
				return false;

			if (isalpha(secondLastChar) && length < 3)
				return false;

			if (isalpha(lastChar))
			{
				char c = tolower(lastChar);
				if (c != 'b' && c != 'k' && c != 'm' && c != 'g')
					return false;
			}

			return true;
		}

		// Convert size from gigabyte, megabyte or kilobyte to byte
		// size = 8, 4, 787897987
		// unit = b, k, m, g
		// ~ = Flip bits
		// ~(0000 0000) = 1111 1111 = UL MAX
		unsigned long	ConfigFile::convertSize(unsigned long &size, char unit)
		{
			switch (tolower(unit))
			{
				case 'b':
					return size;
				case 'k':
					return size *= 1024;
				case 'm':
					return size *= 1024 * 1024;
				case 'g':
					return size *= 1024 * 1024 * 1024;
				default:
					errors_.push_back("Error in convertSize. Invalid unit. You should never see this error.");
					return ~(0UL);
			}
		}

		// only one per block location. If no block location, 
		// only one per server block.
		unsigned long	ConfigFile::parseMaxBodySize(std::string & line)
		{
			unsigned long	max_body_size = 0;
			char			unit = '0';

			if (firstWord(line) == "max_body_size")
			{
				if (line == "max_body_size")
					errors_.push_back("Error: no max_body_size found. Check line : " + line);

				// check IP address
				std::size_t found = line.find("max_body_size "); // this does not work if there is more than 1 space
				if (found != std::string::npos)
				{
					std::string size = line.substr(found + 14);
					if (isValidSize(size) == 0)
						errors_.push_back("Error: Invalid size (" + size + "). Excepted format : 10B, 10b, 200K, 200k, 600m, 600M, 1g, 1G.\nCheck line : " + line);
					else
					{
						size_t l = size.size();
						unit = size[l - 1];
						size.erase(l - 1, l);

						char* endptr;
						max_body_size = strtoul(size.c_str(), &endptr, 10);
					
						if (endptr == size.c_str() || *endptr != '\0')
						{
							std::cerr << "Invalid input.\n";
							return 1;
						}
						convertSize(max_body_size, unit);
					}
				}
			}
			return max_body_size;
		}

		void	getMethods(std::string const & path, Location::vector_type & methods)
		{		
			// split line on every space
			std::stringstream ss(path);
			std::string method;
			while (ss >> method)
			{
				for (size_t i = 0; i < method.size(); i++)
					method.at(i) = std::toupper(method.at(i)); // c'est dégeulasse mais ca m'a saoulée
				methods.push_back(method);
			}

			// get rid of path to keep only methods
			methods.erase(methods.begin());

			// for (std::vector<std::string>::iterator it = methods.begin(); it != methods.end(); ++it)
				// std::cout << ORANGERED1 << *it << RESET << std::endl;
		}

		bool	methodsAreValid(Location::vector_type methods)
		{
			// for (std::vector<std::string>::iterator it = methods.begin(); it != methods.end(); ++it)
				// std::cout << ORANGERED1 << *it << RESET << std::endl;

			for (std::vector<std::string>::iterator it = methods.begin(); it != methods.end(); ++it)
			{
				if (*it != "GET" && *it != "POST" && *it != "DELETE")
					return false;
			}
			return true;
		}

		// location [endroit] <méthodes> {}
		// method is not mandatory but if it is there it must be :
		// GET and/or POST and/or DELETE (separated by space if several)
		std::string	ConfigFile::parseLocation(std::string & line, Server::map_type & locations)
		{
			// std::map<std::string, Location> locations;
			std::string path = "";

			if (firstWord(line) == "location")
			{
				counterServer_["location"]++;
				counterLocation_["location"]++;

				Location location;
			
				if (line == "location")
					errors_.push_back("Error: no location path found. Check line : " + line);
			
				std::size_t found = line.find("location ");
				if (found != std::string::npos)
				{
					path = line.substr(found + 9);
					if (path[0] != '/')
						errors_.push_back("Error : path must start with / . Check line : " + line);
					
					Location::vector_type methods;
					getMethods(path, methods);

					if (hasDuplicates<std::string>(methods))
						errors_.push_back("Error : found duplicates in location methods. Check line : " + line);
					
					if (methodsAreValid(methods) == 0)
						errors_.push_back("Errror: typo in method name. Expected : GET POST DELETE. Check line : " + line);
					else
					{
						std::size_t n = path.find_first_of(" ");
						if (n != std::string::npos)
						{
							std::string endLine = path.substr(n + 1);
							path.erase(n, path.size() - n);
							// delete the / if there is one at the end of filepath
							if (path[path.size() - 1] == '/' && path.size() != 1)
							{
								// std::cout << DEEPPINK3 << "I delete the / at the end of filePath. You're welcome." << RESET << std::endl;
								path.erase(path.end() - 1);
							}
						}
						location.setMethods(methods);
					}
					location.setPath(path);
					locations[path] = location;

					// for (Server::map_type::iterator it = locations.begin(); it != locations.end(); it++)
						// std::cout << it->second << std::endl;
				}
			}
			return path;
		}

		template <typename T>
		bool ConfigFile::hasDuplicates(const std::vector<T> & vec)
		{
			std::vector<T> temp = vec;
			std::sort(temp.begin(), temp.end());
			for (typename std::vector<T>::size_type i = 1; i < temp.size(); ++i)
			{
				if (temp[i] == temp[i - 1])
					return true;
			}
			return false;
		}

		Location::vector_type   ConfigFile::parseIndex(std::string & line)
		{
			Location::vector_type indexes;

			if (firstWord(line) == "index")
			{
				if (line == "index")
					errors_.push_back("Error: no index found. Check line : " + line);
				
				size_t found = line.find("index ");
				if (found != std::string::npos)
				{
					std::string allIndex = line.substr(found +  6);

					// split allIndex on every space
					std::stringstream ss(allIndex);
					std::string index;
					while (ss >> index)
						indexes.push_back(index);
				}

				// check for duplicates
				if (hasDuplicates<std::string>(indexes) == 1)
					errors_.push_back("Error: found duplicate in index property. Check line : " + line);
			}
			return indexes;
		}

		std::vector<std::string>   ConfigFile::parseUploadDir(std::string & line)
		{
			std::vector<std::string> upload_dir;

			if (firstWord(line) == "upload_dir")
			{
				if (line == "upload_dir")
					errors_.push_back("Error: no argument found. Check line : " + line);
				
				size_t found = line.find("upload_dir ");
				if (found != std::string::npos)
				{
					std::string allUpload = line.substr(found +  11);
					// split allUpload on every space
					std::stringstream ss(allUpload);
					std::string upload;
					while (ss >> upload)
						upload_dir.push_back(upload);
					if (upload_dir.size() != 2) 
					{
						errors_.push_back("Error : wrong syntax. Expected : upload_dir <endpoint> <uploadRoot>");
						return upload_dir;
					}
				}
				else
					errors_.push_back("Error : wrong syntax. Expected : upload_dir <endpoint> <uploadRoot>");
			}
			return upload_dir;
		}

		// parse cgi property
		void    ConfigFile::parseCgi(std::string & line, Server::cgi_type & cgi)
		{
			if (firstWord(line) == "cgi")
			{				
				if(line == "cgi")
					errors_.push_back("Error: no cgi found. Check line : " + line );
				
				size_t found = line.find("cgi ");
				if (found != std::string::npos)
				{
					std::string allCgi = line.substr(found + 4);

					// split allCgi on every space
					std::stringstream ss(allCgi);
					std::string cgi_args;
					std::vector<std::string> tmp;
					while (ss >> cgi_args)
						tmp.push_back(cgi_args);
					if (tmp.size() > 3)
						errors_.push_back("Error: expecting only 3 arguments. Format : cgi <extension> <command> (opt)<timeout>. Check line : " + line);
					if (cgi.find(tmp[0]) != cgi.end())
						errors_.push_back("Error: found duplicate. Extension can only be associated with one command. Check line : " + line);
					
					// if timeout is specified
					unsigned long timeout = 0;
					if (tmp.size() > 2)
					{
						if (tmp[2][0] == '-')
							errors_.push_back("Error: timeout overflow. Setting timeout to default value (=2000). Please check line = " + line);

						char *endptr;
						timeout = strtoul(tmp[2].c_str(), &endptr, 10);
						if (*endptr != '\0')
						{
							errors_.push_back("Error: syntax error in timeout value. Setting timeout to default value (=2000). Please check line = " + line);
							timeout = 2000;
						}
					}
					else
						timeout = 2000;
					std::pair<std::string, unsigned long> args(tmp[1], timeout);
					cgi[tmp[0]] = args;
				}
			}
		}

		// parse directory_index property
		// the directory_index property is meant to list the content of the current folder
		// if none of the files specified in the index property are found (or if no index property specified),
		// and if it is set to 1.
		bool    ConfigFile::parseDirectoryIndex(std::string & line, bool & defined)
		{
			bool directory_index = true;
			if (firstWord(line) == "directory_index")
			{
				if (line == "directory_index")
					return true;
				
				int occur = 0;
				for (size_t i = 0; i < line.length(); ++i)
				{
					if (line[i] == ' ')
						occur++;
					if (occur > 1)
					{
						errors_.push_back("Error: syntax error. Format : \"directory_index <bool>\" Check line : " + line);
						break;
					}
				}
				size_t found = line.find("directory_index ");
				if (found != std::string::npos)
				{
					std::string res = line.substr(found + 16);
					// remove whatever from ';'
					size_t n = res.find_first_of(';');
					if (n != std::string::npos)
						res.erase(n, res.size() - n);
					if (res == "true")
						directory_index = true;
					else if (res == "false")
						directory_index = false;
					else
						errors_.push_back("Error: syntax error. Format : \"directory_index <bool>\" Check line : " + line);
				}
				defined = true;
			}
			else
				defined = false;
			return directory_index;
		}

		bool    ConfigFile::parseAllowUpload(std::string & line)
		{
			bool allow_upload = false;
			if (firstWord(line) == "allow_upload")
			{
				counterLocation_["allow_upload"]++;
				if (line == "allow_upload")
					return false;
				
				int occur = 0;
				for (size_t i = 0; i < line.length(); ++i)
				{
					if (line[i] == ' ')
						occur++;
					if (occur > 1)
					{
						errors_.push_back("Error: syntax error. Format : \"allow_upload <bool>\" Check line : " + line);
						break;
					}
				}
				size_t found = line.find("allow_upload ");
				if (found != std::string::npos)
				{
					std::string res = line.substr(found + 13);
					if (res == "true")
						allow_upload = true;
					else if (res == "false")
						allow_upload = false;
					else
						errors_.push_back("Error: syntax error. Format : \"allow_upload <bool>\" Check line : " + line);
				}
			}
			return allow_upload;
		}

		// parse root property
		std::string    ConfigFile::parseRoot(std::string & line)
		{
			std::string root;

			if (firstWord(line) == "root")
			{
				// counterServer_["root"]++;

				if (line == "root")
					errors_.push_back("Error: no root path found. Check line : " + line);

				size_t found = line.find("root ");
				if (found != std::string::npos)
					root = line.substr(found + 5);
			}
			return root;
		}

		// parse redirect property and push it in a map --> map[code] = pair(old/url, new/url)
		Server::redirect_type	ConfigFile::parseRedirect(std::string & line, Server::redirect_type & redirect)
		{
			// Server::redirect_type	redirect;

			if (firstWord(line) == "redirect")
			{
				counterServer_["redirect"]++;

				if (line == "redirect")
					errors_.push_back("Error: no redirect arguments found. Check line : " + line);

				size_t found = line.find("redirect ");
				if (found != std::string::npos)
				{
					std::string err_string = line.substr(found + 9);

					std::stringstream ss(err_string);
					std::string redirect_args;
					std::vector<std::string> tmp;
					while (ss >> redirect_args)
						tmp.push_back(redirect_args);
					if (tmp.size() != 3)
					{
						errors_.push_back("Error: expecting 3 arguments. Format : redirect <err_code> <old/url> <new/url>. Check line : " + line);
						return redirect;
					}
					else if (stringIsDigit(tmp[0]) == false)
					{
						errors_.push_back("Error: first argument should be un integer. Format : redirect <err_code> <url>. Check line : " + line);
						return redirect;
					}
					int code = atoi(tmp[0].c_str());
					if (code < 300 || code > 399)
						errors_.push_back("Error: error code should be between 300 and 399. Check line : " + line);
					redirect[tmp[1]] = std::make_pair(code, tmp[2]);

					// FOR DEBUG					
					// for (Server::redirect_type::iterator it = redirect.begin(); it != redirect.end(); it++) {
						// std::cout << "redirect[ " << it->first << " ] = " << it->second.first << " | " << it->second.second << std::endl;
					// }
				}
			}
			return redirect;
		}

		// parse error_page property, a pair with error_code (int) and url (std::string)
		std::map<int, std::string>	ConfigFile::parseErrorPage(std::string & line, std::map<int, std::string> & error_page)
		{
			if (firstWord(line) == "error_page")
			{
				counterServer_["error_page"]++;

				if (line == "error_page")
					errors_.push_back("Error: no error_page arguments found. Check line : " + line);

				size_t found = line.find("error_page ");
				if (found != std::string::npos)
				{
					std::string err_string = line.substr(found + 11);

					std::stringstream ss(err_string);
					std::string error_page_args;
					std::vector<std::string> tmp;
					while (ss >> error_page_args)
						tmp.push_back(error_page_args);
					if (tmp.size() > 2)
						errors_.push_back("Error: expecting only two arguments. Format : error_page <err_code> <url>. Check line : " + line);
					else if (stringIsDigit(tmp[0]) == false)
						errors_.push_back("Error: first argument should be un integer. Format : error_page <err_code> <url>. Check line : " + line);
					std::pair<int, std::string> err_page;
					err_page.first = atoi(tmp[0].c_str());
					if (err_page.first < 400 || err_page.first > 499)
						errors_.push_back("Error: error code should be between 400 and 499. Check line : " + line);
					err_page.second = tmp[1];
					if (error_page.insert(err_page).second == false)
						errors_.push_back("Error: found duplicates in error codes for error_page properties. Check line : " + line);
				}
			}
			return error_page;
		}

		// requirements :
		// server_name : at least one / max : one per Server
		// listen : at least one / max : 1 per Server
		// max_body_size : at least 0 / max : 1 per Server, 1 per Location
		// location : at least 0 / max : +inf
		// index : at least 0 / max : 1 per Location, 1 per Server (max_body_size ==)
		// cgi : at least 0 / max : +inf
		// directory_index : at least 0 / max : 1 per Location 1 per Server (max_body_size == , default = true)
		// root : at least one / max : 1 per Server (default = front)
		void    ConfigFile::verifyOccurence(std::string block)
		{
			std::map<std::string, int> counter;


			if (block == "server")
				counter = counterServer_;
			else
			{
				counter = counterLocation_;
			}

			// std::cout << "Checking for : " << block << std::endl;
			// std::cout << "server = " << counter["server"] << std::endl;
			// std::cout << "server_name = " << counter["server_name"] << std::endl;
			// std::cout << "root = " << counter["root"] << std::endl;
			// std::cout << "listen = " << counter["listen"] << std::endl;
			// std::cout << "max_body_size = " << counter["max_body_size"] << std::endl;
			// std::cout << "index = " << counter["index"] << std::endl;
			// std::cout << "directory_index = " << counter["directory_index"] << std::endl;
			// std::cout << "location = " << counter["location"] << std::endl;
			// std::cout << "cgi = " << counter["cgi"] << std::endl;
			// std::cout << "redirect = " << counter["redirect"] << std::endl;
			// std::cout << "allow_upload = " << counter["allow_upload"] << std::endl;


			if (block == "server")
			{
				if (counter["server_name"] > 1)
					errors_.push_back("Error : you should have one entry of \"server_name\" per server block.");

				if (counter["root"] != 1)
					errors_.push_back("Error : you should have one entry of \"root\" per server block. Root property is mandatory.");

				if (counter["listen"] != 1)
					errors_.push_back("Error : you should have one entry of \"listen\" per server block. Listen property is mandatory.");
				
				if (counter["max_body_size"] > 1)
					errors_.push_back("Error : you should have one entry of \"max_body_size\" max per server block.");
				
				if (counter["index"] > 1)
					errors_.push_back("Error : you should have one entry of \"index\" max per server block.");

				if (counter["directory_index"] > 1)
					errors_.push_back("Error : you should have one entry of \"directory_index\" max per server block.");
				
				// if (counter["redirect"] > 1)
					// errors_.push_back("Error : you should have one entry of \"redirect\" max per server block.");
				
				// if (counter["upload_dir"] != 1)
					// errors_.push_back("Error : you should have one entry of \"upload_dir\" max per server block. upload_dir is mandatory.");
			}

			if (block == "location")
			{
				if (counter["max_body_size"] > 1)
					errors_.push_back("Error : you should have one entry of \"max_body_size\" per location block.");
				
				if (counter["directory_index"] > 1)
					errors_.push_back("Error : you should have one entry of \"directory_index\" per location block.");
				
				if (counter["index"] > 1)
					errors_.push_back("Error : you should have one entry of \"index\" per location block.");
				
				// if (counter["upload_dir"] > 1)
					// errors_.push_back("Error : you should have one entry of \"upload_dir\" max per location block.");

				if (counter["root"] > 1)
					errors_.push_back("Error : you should have one entry of \"root\" max per location block.");
				
				if (counter["allow_upload"] > 1)
					errors_.push_back("Error : you should have one entry of \"allow_upload\" max per location block.");

			}
		}

		void	ConfigFile::produceConfigLogs()
		{
			// copy content of errors_ into log file
			std::ofstream file("configLogs"); 
			
			if (file.is_open())
			{
				std::vector<std::string>::iterator it = errors_.begin();
				for(; it != errors_.end(); it++)
					file << *it << std::endl;
				file.close(); // close the file
			}
			else
				std::cout << "Unable to create file." << std::endl; // display error message if unable to create file
		}

		// returns wether of not we reached the end of server block.
		// Only two possibilities next line that is not commented or empty is :
		//  * case 1) the start of another server block
		//  * case 2) the end of the vector lines_ 
		bool	ConfigFile::reachedEndOfServerBlock(std::vector<std::string>::const_iterator it)
		{
			std::vector<std::string>::const_iterator copy = it;
			
			if (it + 1 != lines_.end())
				copy = it + 1;
			else
				return true;

			while (copy != lines_.end() && (((*copy).empty() == 1 || (*copy)[0] == '#') || onlyWhiteSpaces(*copy)))
				copy++;

			if (copy == lines_.end())
				return true;
			if (*copy == "server")
				return true;
			return false;
		}

		void	replaceMaxBodySize(Server::map_type & locations, Server & new_server)
		{
			for(Server::map_type::iterator it = locations.begin(); it != locations.end(); it++)
			{
				if (it->second.getMaxBodySize() == 0)
					it->second.setMaxBodySize(new_server.getMaxBodySize());
			}
		}

		void	replaceIndex(Server::map_type & locations, Server & new_server)
		{
			for(Server::map_type::iterator it = locations.begin(); it != locations.end(); it++)
			{
				if (it->second.getIndex().size() == 0)
					it->second.setIndex(new_server.getIndex());
			}
		}

		// void	replaceUploadDir(Server::map_type & locations, Server & new_server)
		// {
		// 	for(Server::map_type::iterator it = locations.begin(); it != locations.end(); it++)
		// 	{
		// 		if (it->second.getUploadDir().size() == 0)
		// 			it->second.setUploadDir(new_server.getUploadDir());
		// 	}
		// }

		void	replaceRoot(Server::map_type & locations, Server & new_server)
		{
			for(Server::map_type::iterator it = locations.begin(); it != locations.end(); it++)
			{
				if (it->second.getRoot() == "")
					it->second.setRoot(new_server.getRoot());
			}
		}

		void	replaceDirectoryIndex(Server::map_type & locations, Server & new_server)
		{
			for(Server::map_type::iterator it = locations.begin(); it != locations.end(); it++)
			{
				if (it->second.getDirectoryIndexWasSet() == 0)
					it->second.setDirectoryIndex(new_server.getDirectoryIndex());
			}
		}

		bool	isInServerNamesAddons(std::string name, std::vector<std::string> & ServerNamesAddons) {
			for (size_t i = 0; i < ServerNamesAddons.size(); i++) {
				if (name == ServerNamesAddons[i])
					return true;
			}
			return false;
		}

		bool	ConfigFile::foundListenDuplicate(std::vector<std::string> & ServerNamesAddons)
		{
			std::vector<int> remember;
			for(std::map<std::string, Server>::iterator it = servers.begin(); it != servers.end(); it++) {
				if (isInServerNamesAddons(it->second.getServerName(), ServerNamesAddons) == 0) {
					remember.push_back(it->second.getPort());
				}
			}

			for (size_t i = 1; i < remember.size(); i++)
			{
				if (remember[i] == remember[i - 1])
					return true;
			}
			return false;
		}

		bool	ConfigFile::onlyWhiteSpaces(std::string const & line)
		{
			for (size_t i = 1; i < line.size(); i++)
			{
				if (!std::isspace(line[i])) 
					return false;
			}
			return true;
		}

		// returns indent of next line, ot -1 if reached end of file
		int	ConfigFile::getNextLineIndent(std::vector<std::string>::const_iterator it)
		{
			std::vector<std::string>::const_iterator copy = it;
			
			if (it + 1 != lines_.end())
				copy = it + 1;
			else
				return -1;

			while (copy != lines_.end() && (((*copy).empty() == 1 || (*copy)[0] == '#') || onlyWhiteSpaces(*copy)))
				copy++;
			
			if (copy != lines_.end())
				return (*copy).find_first_not_of("\t");
			return -1;
		}

		// check if first word of line matches one of the properties we are prepared to parse.
		bool	propertyExists(std::string firstWord)
		{
			std::vector<std::string> properties;
			properties.push_back("server");
			properties.push_back("server_name");
			properties.push_back("listen");
			properties.push_back("max_body_size");
			properties.push_back("location");
			properties.push_back("index");
			properties.push_back("cgi");
			properties.push_back("directory_index");
			properties.push_back("root");
			properties.push_back("redirect");
			properties.push_back("error_page");
			// properties.push_back("upload_dir");
			properties.push_back("allow_upload");

			for(std::vector<std::string>::iterator it = properties.begin(); it != properties.end(); it++)
				if (firstWord == *it)
					return true;
			return false;
		}

		// check if the indent of the property is correct
		// an exclusively server-block property should have an indent equal of inferior to the one of Server;
		// and should not have an indent greater than the one of location.
		bool indentIsRight(std::string firstWord, int indent, int indentLocation, int indentServer)
		{
			std::vector<std::string> onlyServerProp;
			onlyServerProp.push_back("server_name"); // only server
			onlyServerProp.push_back("listen"); // only server
			onlyServerProp.push_back("location"); // only server
			onlyServerProp.push_back("cgi"); // only server
			onlyServerProp.push_back("redirect"); // only server
			onlyServerProp.push_back("error_page"); // only server
			// onlyServerProp.push_back("upload_dir"); // only server


			for(std::vector<std::string>::iterator it = onlyServerProp.begin(); it != onlyServerProp.end(); it++)
			{
				if (firstWord == *it)
				{
					// std::cout << indent << " | " << indentLocation << " | " << indentServer << std::endl;
					if (indent <= indentServer || (indentLocation != -3 && indent > indentLocation))
						return false;
				}
			}
			return true;
		}

		// void	addUploadDirToLocation(std::vector<std::string> & upload_dir, Server::map_type & locations, std::string ServeRoot)
		// {
		// 	if (upload_dir.size() == 2)
		// 	{
		// 		Location loc;
		// 		loc.setDirectoryIndex(1);
		// 		loc.setPath(upload_dir[1]);
		// 		loc.setRoot(ServeRoot);
		// 		Location::vector_type tmp;
		// 		tmp.push_back("DELETE");
		// 		tmp.push_back("POST");
		// 		loc.setMethods(tmp);
		// 		locations[upload_dir[1]] = loc; 
		// 	}
		// }

		void	saveServerNamesAddons(std::vector<std::string> server_name, std::vector<std::string> & ServerNamesAddons) {
			for (size_t i = 1; i < server_name.size(); i++) {
				ServerNamesAddons.push_back(server_name[i]);
			}

			//FOR DEBUG
			// for (size_t i = 0; i < ServerNamesAddons.size(); i++) {
				// std::cout << "ServerNamesAddons = " << ServerNamesAddons[0] << std::endl;
			// }
		}

		bool	ConfigFile::parseFile()
		{
			bool inServerBlock = false;
			bool inLocationBlock = false;
			bool valid = true;

			int indent = 0; // initialize indent level
			int indentServer = -1;
			int indentLocation = -3;

			// tool to check if boolean value directoryIndex is defined
			bool defined = false;

			Server						new_server;
			std::vector<std::string>	server_name;
			std::pair<std::string, int>	listen;
			Server::cgi_type 			cgi;
			Server::map_type 			locations;
			Server::redirect_type		redirect;
			std::map<int, std::string>	error_page;
			Location::vector_type 		indexes;
			unsigned long 				max_body_size = 0;
			bool 						directoryIndex = true;
			std::string 				path;
			std::string 				root;
			// std::vector<std::string>	upload_dir;
			std::vector<std::string>	ServerNamesAddons;

			initReqServer();
			initReqLocation();

			for (std::vector<std::string>::const_iterator it = lines_.begin(); it != lines_.end(); ++it)
			{
				std::string line = *it;
				// FOR DEBUG
				// std::cout << MEDIUMPURPLE << line << RESET << std::endl;
				
				// Strip whitespace at the end of the line
				line.erase(line.find_last_not_of(" \t") + 1);
				replaceTabs(line);
				trimSpaces(line);

				// Ignore comments and empty lines
				if (line.empty() || line[0] == '#')
					continue;

				// check if first line initiate server block
				else if (lines_[0].find("server") == std::string::npos)
					errors_.push_back("Error: configuration file should start with server block. Check line : " + line);

				// find how many tabs at beginning of line. If empty line returns -1.
				indent = line.find_first_not_of("\t");

				// if empty line
				if (indent == -1)
					indent = indentServer;
				
				// count server blocks
				else if (line == "server")
				{
					counterServer_["server"]++;
					indentServer = indent;
					inServerBlock = true;
				}
				
				if (indent > indentServer)
					inServerBlock = true;
				else if (indent <= indentServer && inServerBlock)
					inServerBlock = false;

				// if indent level is lower than indentLocation, we are outside of a location block
				if (indent <= indentLocation && inLocationBlock)
				{
					inLocationBlock = false;
					verifyOccurence("location");
					initReqLocation();
 				}
				
				if (firstWord(line) == "server_name")
					parseServerName(line, server_name);

				else if (firstWord(line) == "listen")
					listen = parseListen(line);
				
				// else if (firstWord(line) == "root")
					// root = parseRoot(line);

				else if (firstWord(line) == "redirect")
					parseRedirect(line, redirect);
				
				else if (firstWord(line) == "cgi")
					parseCgi(line, cgi);
				
				else if (firstWord(line) == "error_page")
					parseErrorPage(line, error_page);

				// if we are not in a location block
				if (inLocationBlock == 0)
				{
					if (firstWord(line) == "root")
					{
						root = parseRoot(line);
						counterServer_["root"]++;
					}
					if (firstWord(line) == "max_body_size")
					{
						max_body_size = parseMaxBodySize(line);
						counterServer_["max_body_size"]++;
					}

					if (firstWord(line) == "directory_index")
					{
						directoryIndex = parseDirectoryIndex(line, defined);
						counterServer_["directory_index"]++;
					}

					if (firstWord(line) == "index")
					{
						indexes = parseIndex(line);
						counterServer_["index"]++;
					}

					// if (firstWord(line) == "upload_dir")
					// {
						// upload_dir = parseUploadDir(line);
						// counterServer_["upload_dir"]++;
					// }
				}

				// if we are in a location block (2 conditions : first line starts with "location", 
				// indent level is higher than indentServer)
				if (firstWord(line) == "location")
				{
					if (indent <= indentServer)
						errors_.push_back("Error : location block must be in server block. Verify indent level. Check line : " + line);
						
					// update indentLocation
					indentLocation = indent;

					// we are in a LocationBlock
					inLocationBlock = true;

					// check if location block is empty
					if (getNextLineIndent(it) <= indentLocation)
						errors_.push_back("Error : empty location block. Check line : " + line);

					path = parseLocation(line, locations);

					// FOR DEBUG
					// for (Server::map_type::iterator it = locations.begin(); it != locations.end(); it++)
					// 	std::cout << it->second << std::endl;
				}

				if (propertyExists(firstWord(line)) == 0)
					errors_.push_back("Error : unknown property. Please check line : " + line);
				else if (indentIsRight(firstWord(line), indent, indentLocation, indentServer) == 0)
					errors_.push_back("Error : mind line indent. It defines your blocks. Check line : " + line);
				
				// if we are in a location block
				if (indent > indentLocation && inLocationBlock)
				{
					// set root
					std::string root_index = parseRoot(line);
					if (root_index != "")
					{
						counterLocation_["root"]++;
						locations[path].setRoot(root_index);
					}
					// set max_body_size_index
					unsigned long max_body_size_index = parseMaxBodySize(line);
					if (max_body_size_index == 0)
						locations[path].setMaxBodySize(max_body_size);
					else
					{
						counterLocation_["max_body_size"]++;
						locations[path].setMaxBodySize(max_body_size_index);
					}

					// set index
					Location::vector_type indexes = parseIndex(line);
					if (indexes.size() != 0)
					{
						counterLocation_["index"]++;
						locations[path].setIndex(indexes);
					}

					// set directoryIndex
					bool directory_index = parseDirectoryIndex(line, defined);
					if (defined)
					{
						counterLocation_["directory_index"]++;
						locations[path].setDirectoryIndex(directory_index);
					}

					// set allow_upload
					if (firstWord(line) == "allow_upload") {
						bool allow_upload = parseAllowUpload(line);
						locations[path].setAllowUpload(allow_upload);
					}
					// set upload_dir
					// std::vector<std::string> upload_dir = parseUploadDir(line);
					// if (upload_dir.size() != 0)
					// {
						// counterLocation_["upload_dir"]++;
						// locations[path].setUploadDir(upload_dir);
					// }
				}

				// if we are at the end of the current server block
				if (reachedEndOfServerBlock(it))
				{
					// FOR DEBUG
					// std::cout << RED1 << "YOUHOUUUU" << RESET << std::endl;
					// for (Server::map_type::iterator it = locations.begin(); it != locations.end(); it++)
						// std::cout << it->second << std::endl;
					// std::cout << new_server << std::endl;

					// If they are not specified in a Location block, 
					// these Server block values will define those of  
					// the same title in a location block.
					new_server.setIndex(indexes);
					// new_server.setUploadDir(upload_dir);
					new_server.setDirectoryIndex(directoryIndex);
					new_server.setMaxBodySize(max_body_size);
					new_server.setRoot(root);

					// if the following properties were specified AFTER 
					// a(some) location(s) block(s), we update the locations objects
					replaceMaxBodySize(locations, new_server);
					replaceDirectoryIndex(locations, new_server);
					replaceIndex(locations, new_server);
					// replaceUploadDir(locations, new_server);
					replaceRoot(locations, new_server);
					
					new_server.setRedirect(redirect);
					new_server.setErrorPage(error_page);
					// new_server.setRoot(root);
					new_server.setIP(listen.first);
					new_server.setPort(listen.second);
					// addUploadDirToLocation(upload_dir, locations, new_server.getRoot());

					new_server.setLocations(locations);
					new_server.setCgi(cgi);
					
					// if no server_name property is found, we'll setup a name ourselves,
					// using the server's port which must always be unique to
					// each server blocks.
					if (server_name.size() == 0) {
						server_name.push_back("_");
					}

					// update servers with new Server block info 
					for(size_t i = 0; i < server_name.size(); i++)
					{
						std::pair<map_type::iterator, bool> output;
						new_server.setServerName(server_name[i]);
						output = servers.insert(make_pair(server_name[i], new_server));
						if (output.second == false)
							errors_.push_back("Error : cannot have twice the same server name. Make your choice.");
					}
					
					// reset variables
					locations.clear();
					cgi.clear();
					indexes.clear();
					directoryIndex = true;
					max_body_size = 8388608;
					redirect.clear();
					error_page.clear();
					// upload_dir.clear();
					
					saveServerNamesAddons(server_name, ServerNamesAddons);
					server_name.clear();
					
					verifyOccurence("server");
					initReqServer();
				}
			}

			// if (foundListenDuplicate(ServerNamesAddons))
			// 	errors_.push_back("Error : two server blocks cannot have the exact same port or IP in listen property.");

			// print map of servers - FOR DEBUG
			for(ConfigFile::map_type::iterator it = servers.begin(); it != servers.end(); it++)
			{
				std::cout << SALMON1 << ">>>> Printing server with key : " << it->first << std::endl;
				std::cout << it->second << std::endl;
				std::cout << "-------------------------------------" << std::endl;
			}

			if (errors_.size() > 0)
				valid = false;
			return valid;
		}
	}
}
