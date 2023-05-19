#include "Response.hpp"
#include "Socket.hpp"
#include "Cgi.hpp"
#include "logger.hpp"
#include "exceptions.hpp"
#include "constant.hpp"
#include "utils.hpp"
#include "codes.hpp"
#include "configFile.hpp"
#include <signal.h>
#include <unistd.h>
#include <iostream>
#include <cstring>
#include <cstdio>
#include <set>

// CONFIG IMPLEMENTATION - START
#include <sys/types.h>
#include <sys/stat.h>
// CONFIG IMPLEMENTATION - END
#include "constant.hpp"


#define URI_MAX_LENGTH 2048             // Maximum length of the URI (2 kB)
#define REQUEST_HEADERS_MAX_SIZE 16384  // Maximum size of all the request headers (16 kB)

static bool running = true;
using namespace webserv::http::codes;
using namespace webserv::config;

void gracefulExit(int sig, siginfo_t *siginfo, void *context)
{
	(void)sig;
	(void)siginfo;
	(void)context;
	webserv::logger::info("[socket] Gracefully exiting");
	running = false;
}

bool dirExistsOrCreate(std::string const &dir)
{
	struct stat fileStat;
	if (stat(dir.c_str(), &fileStat) != 0)
	{
		if (mkdir(dir.c_str(), 0777) != 0)
		{
			webserv::logger::error("[POST] Make upload directory error: " + dir);
			return false;
		}
		else
		{
			webserv::logger::info("[POST] Make upload directory success: " + dir);
			return true;
		}
	}
	if (S_ISDIR(fileStat.st_mode) != 1)
	{
		return false;
	}
	return true;
}

void	checkUploadDir(std::string const &method, webserv::config::Server &server, Location &loc, std::string &dir)
{
	bool	isAllow = false;

	if (method != "POST")
			return ;
	isAllow = loc.getAllowUpload();
	if (isAllow == false)
		throw webserv::exceptions::http::InvalidUploadDir();
	dir = webserv::utils::path_join(server.getRoot(), loc.getPath(), '/');
	if (dirExistsOrCreate(dir) == false)
		throw webserv::exceptions::http::InvalidUploadDir();
}

void shiftBoundaryBuf(std::string &buf, size_t &readSize, size_t nShift, size_t &posStart, size_t &posEnd)
{
    buf.erase(0, nShift);
    readSize -= nShift;
    if (posEnd != std::string::npos) // UPDATE BODY END DUE TO SHIFT WITH ERASE
        posEnd = posEnd - nShift;
    posStart = 0; // UPDATE BOUNDARY START DUE TO SHIFT WITH ERASE
}

size_t  getPosContentPart(webserv::http::Response &response, std::string &buf, std::vector<size_t> &readSizeSave, size_t &readSize, const char *needle)
{
    char buffer[BODY_BUFFER_SIZE];
    size_t  res = buf.find(needle);

    while (res == std::string::npos && readSize > 0)
    {
        readSize = response.request.readBody(buffer, BODY_BUFFER_SIZE);
        buffer[readSize] = '\0';
        buf.append(buffer, readSize);
        memset(buffer, '\0', sizeof(buffer)); // REINIT buffer FOR THE NEXT ITERATION
        res = buf.find(needle);
        readSizeSave.push_back(readSize);
    }
    return (res);
}

bool requestHeaderFileTooLarge(webserv::http::Response &response)
{
	size_t headersSize = 0;
	for (std::map<std::string, std::string>::iterator it = response.request.headers.begin(); it != response.request.headers.end(); it++)
	{
		headersSize += it->first.length() + it->second.length();
	}
	if (headersSize > REQUEST_HEADERS_MAX_SIZE)
	{
		return true;
	}
	return false;
}

bool fileTraversalExploitDetected(webserv::http::Response &response)
{
	if (response.request.path.find("../") != std::string::npos)
	{
		return true;
	}
	return false;
}

bool rootExists(webserv::http::Response &response, webserv::config::Server server)
{

	webserv::logger::debug("[USE CASE 3] root existe ?");
	webserv::logger::debug("[USE CASE 3] request path: " + response.request.path);
	webserv::logger::debug("[USE CASE 3] root = " + server.getRoot());

	struct stat fileStat;
	if (stat(server.getRoot().c_str(), &fileStat) != 0)
	{
		webserv::logger::debug("[USE CASE 3] root does not exist");
		return false;
	}
	if (S_ISDIR(fileStat.st_mode) != 1)
	{
		webserv::logger::debug("[USE CASE 3] root does not exist");
		return false;
	}
	webserv::logger::debug("[USE CASE 3] root exists!");
	return true;
}

std::string getFolderPath(webserv::http::Response &response)
{
	std::string folder_path = response.request.path;

	// extract folder_path in request and push it in folder_path
	size_t pos = folder_path.find_last_of("/");
	if (pos != std::string::npos)
	{
		// 1 --> extract string from last occurence of / to the end
		std::string tmp = folder_path.substr(pos);

		// 2 --> if there is a dot in tmp, aka request is done on a file,
		// we want to get rid of the file and keep only the folder path
		size_t dot_pos = tmp.find('.');
		if (dot_pos != std::string::npos)
		{
			// extract string from beginning of the path to / excluded
			folder_path = folder_path.substr(0, pos);
			if (folder_path == "")
				folder_path = "/";
		}
	}
	webserv::logger::debug("[USE CASE 4] folder_path = " + folder_path);
	return folder_path;
}

// check if folder_path matches a location set up in config file
Location getLocation(webserv::config::Server server, std::string folder_path)
{
	std::map<std::string, Location>::const_iterator locationsIterator = server.getLocations().find(folder_path);
	// if no location corresponds, create a default one
	if (locationsIterator == server.getLocations().end())
	{
		Location defaultLoc;
		defaultLoc.setPath("defaultLoc");
		defaultLoc.setRoot(server.getRoot());
		defaultLoc.setIndex(server.getIndex());
		defaultLoc.setMaxBodySize(server.getMaxBodySize());
		webserv::logger::debug("didn't find location key. Set location to default. ");
		return defaultLoc;
	}
	webserv::logger::debug("found location key: " + locationsIterator->first);
	return locationsIterator->second;
}

void checkURILength(webserv::http::Response &response)
{
	if (response.request.uri.length() > URI_MAX_SIZE)
	{
		throw webserv::exceptions::http::UriTooLong();
	}
}

void checkMethod(Location loc, std::string method)
{
	std::vector<std::string> methods = loc.getMethods();
	std::vector<std::string>::const_iterator methodsIterator = methods.begin();
	for (; methodsIterator != methods.end(); ++methodsIterator)
	{
		webserv::logger::debug("[USE CASE 4] methodsIterator = " + *methodsIterator);
		if (*methodsIterator == method)
		{
			webserv::logger::debug("[USE CASE 4] the method specified in config file for this location matches the one in the resquest");
			break;
		}
	}
	if (methodsIterator == methods.end())
	{
		webserv::logger::error("[response] the requested method for this location : [ " + loc.getPath() + " ] is not allowed by config file.");
		throw webserv::exceptions::http::InvalidCfgLocation();
	}
}

void checkContentLength(webserv::http::Response &response, std::string method, Location loc)
{
	webserv::logger::debug("[USE CASE 5] if the requested method is POST, let's check if Content-Length is > max_body_size");
	if (method == "POST")
	{
		std::istringstream iss(response.request.getHeader("Content-Length"));
		unsigned long len = 0;
		if (iss >> len)
		{
			if (len > loc.getMaxBodySize())
				throw webserv::exceptions::http::ContentLengthTooLong();
		}
		else
		{
			throw webserv::exceptions::http::ConversionFailure();
		}
	}
}

void checkIfDirectoryIndexIsOn(std::string method, webserv::config::Server server, webserv::http::Response &response, Location loc)
{
	webserv::logger::debug("[USE CASE 6] if the request is done on a folder, and the requested method is GET");
	webserv::logger::debug("[USE CASE 6] root = " + server.getRoot());
	webserv::logger::debug("[USE CASE 6] response.request.path = " + response.request.path);

	std::string path;
	size_t found = response.request.path.find('?');
	if (found != std::string::npos)
		path = response.request.path.substr(0, found);
	else
		path = response.request.path;

	if (response.request.virtualPath.length() == 0)
		path = webserv::utils::path_join(server.getRoot(), path);
	else
		path = response.request.virtualPath;
	
	webserv::logger::debug("[USE CASE 6] updated path = " + path);
	struct stat fileStat;
	if (stat(path.c_str(), &fileStat) != 0)
	{
		webserv::logger::error("[response] This path : " + path + ", does not exist.");
		throw webserv::exceptions::http::InvalidPath();
	}
	
	// if the request is done on a folder, and the requested method is GET
	if (S_ISDIR(fileStat.st_mode) == 1 && method == "GET")
	{
		// if there is at least one index specified in location block,
		// serve first index that exists (check is done with access)
		if (loc.getIndex().size() >= 1)
		{
			webserv::logger::debug("[USE CASE 6] serving indexes");
			for (size_t i = 0; i < loc.getIndex().size(); i++)
			{
				std::string path = webserv::utils::path_join(loc.getRoot(), loc.getIndex()[i]);
				// if access == 0, it means the index exists
				if (access(path.c_str(), F_OK) == 0)
				{
					response.file(path);
					return;
				}
			}
		}
		
		if (loc.getDirectoryIndex() == true)
			response.directory(path);
		else if (loc.getDirectoryIndex() == false)
			throw webserv::exceptions::http::InvalidPath();
	}
}

void checkCGI(std::string method, webserv::config::Server server, webserv::http::Response &response)
{
	if (method == "GET")
	{
		std::string path;
		size_t found1 = response.request.path.find('?');
		if (found1 != std::string::npos)
			path = response.request.path.substr(0, found1);
		else
			path = response.request.path;
		std::string extension;
		size_t found = path.find_last_of('.');
		if (found != std::string::npos)
			extension = path.substr(found + 1);
		webserv::config::Server::cgi_type::const_iterator it = server.getCgi().find(extension);
		if (it != server.getCgi().end())
		{
			try
			{
				response.cgi(it->second.first, webserv::utils::path_join(server.getRoot(), response.request.path));
			}
			catch (std::exception &e)
			{
				std::cerr << e.what() << std::endl;
				response.errorPage(GatewayTimeout);
			}
		}
		else
		{
			response.file(webserv::utils::path_join(server.getRoot(), response.request.path));
		}
	}
}

// check if method requested is DELETE and if the path leads to a file or a folder in the /upload folder
// if such is the case,
// delete the file or folder that the path leads to and throw error
// if not
// throw error
void deletePath(std::string method, Location loc, webserv::http::Response &response, std::string root) {
	if (method == "DELETE") {
		std::string path = webserv::utils::path_join(root, response.request.path);
		if (loc.getAllowUpload() == false) {
			webserv::logger::info("[server] I can't delete this file \'" + path 
				+ "\'. 'allow_upload' is set to false for this location : \'" + loc.getPath() 
				+ "\'. If your location is equal to 'defaultLoc', it means the path to the folder you want to modify has not been found in the locations properties of the configuration file.");
			throw webserv::exceptions::http::Forbidden();
		}
		webserv::logger::debug("[server] I will try to delete this file: " + path);
		if (std::remove((path).c_str()) != 0)
			throw webserv::exceptions::http::ErrorDeletingFile();
		else
			webserv::logger::info("[server] I deleted this file successfully : " + response.request.path);
		throw webserv::exceptions::http::ContentOfCurrentPageHasBeenDeleted();
	}
}

// if path matches one of redirect's config property, redirect on page wich code corresponding to the one in config file
int checkRedirect(webserv::http::Response &response, webserv::config::Server server)
{

	webserv::config::Server::redirect_type::const_iterator it = server.getRedirect().find(response.request.path);
	if (it != server.getRedirect().end())
	{
		response.responseHeaders["Location"] = it->second.second;
		return it->second.first;
	}
	else
	{
		return 0;
	}
}

void getLocationRoot(webserv::http::Response &response, Server server, Location loc)
{
	std::string locRoot = loc.getRoot();
	if (locRoot != server.getRoot())
	{
		webserv::logger::debug("[USE CASE 7] root property found for this specific location : " + response.request.path);
		response.request.virtualPath = locRoot;
		webserv::logger::debug("[USE CASE 7] response.request.virtualRoot for : ' " + loc.getPath() + " ' has been set to " + locRoot);
	}
}

void onConnection(webserv::http::Response &response)
{
	try
	{
		webserv::logger::debug("[socket] New connection");
		webserv::config::Server server;

		if (response.request.invalid)
		{
			return response.errorPage(BadRequest);
		}

		if (requestHeaderFileTooLarge(response))
		{
			webserv::logger::error("[response] Request headers too large");
			return response.errorPage(RequestHeaderFieldsTooLarge);
		}

		try
		{
			checkURILength(response);
		}
		catch (std::exception &e)
		{
			std::cerr << e.what() << std::endl;
			return response.errorPage(UriTooLong);
		}

		if (fileTraversalExploitDetected(response))
		{
			webserv::logger::error("[response] File traversal exploit detected");
			return response.errorPage(NotFound);
		}

		webserv::logger::debug("[USE CASE 2] Does the virtual server exist ?");
		webserv::logger::debug("[USE CASE 2] request path: " + response.request.path);
		try
		{
			// TODO: if server is not found, check if `_` exists
			server = response.request.config->getServer(response.request.getHeader("Host"), response.request.port);
		}
		catch (std::runtime_error &e)
		{
			webserv::logger::error("[response] Host does not exist: " + response.request.getHeader("Host"));
			return response.errorPage(NotFound);
		}

		if (!rootExists(response, server))
			return response.errorPage(NotFound);
		
		int code = checkRedirect(response, server);
		if (code != 0)
			return response.errorPage(code);

		std::string method = response.request.method;
		std::string folder_path = getFolderPath(response);
		Location loc = getLocation(server, folder_path);

		getLocationRoot(response, server, loc);

		try
		{
			webserv::logger::debug("[USE CASE 4] Let's check if method is allowed for the requested folder.");
			webserv::logger::debug("[USE CASE 4] request path: " + response.request.path);
			checkMethod(loc, method);
		}
		catch (std::exception &e)
		{
			std::cerr << e.what() << std::endl;
			return response.errorPage(MethodNotAllowed);
		}

		try
		{
			checkContentLength(response, method, loc);
		}
		catch (std::exception &e)
		{
			std::cerr << e.what() << std::endl;
			return response.errorPage(PayloadTooLarge);
		}

		try
		{
			checkIfDirectoryIndexIsOn(method, server, response, loc);
		}
		catch (std::exception &e)
		{
			std::cerr << ORANGERED1 << e.what() << RESET << std::endl;
			return response.errorPage(NotFound);
		}

		try {
			deletePath(method, loc, response, server.getRoot());
		} catch (webserv::exceptions::http::ContentOfCurrentPageHasBeenDeleted &e) {
			std::cerr << e.what() << std::endl;
			return response.errorPage(NoContent);
		} catch (webserv::exceptions::http::Forbidden &e) {
			std::cerr << e.what() << std::endl;
			return response.errorPage(Forbidden);
		}

		try
		{
			checkCGI(method, server, response);
		}
		catch (std::exception &e)
		{
			std::cerr << DARKOLIVEGREEN6 << e.what() << RESET << std::endl;
			return response.errorPage(NotFound);
		}

		try {
			if (method == "POST") {
				std::string extension;
				size_t found = response.request.path.find_last_of('.');
				if (found != std::string::npos)
					extension = response.request.path.substr(found + 1);
				webserv::config::Server::cgi_type::const_iterator it = server.getCgi().find(extension);
				if (it != server.getCgi().end())
				{
					try
					{
						response.cgi(it->second.first, webserv::utils::path_join(server.getRoot(), response.request.path));
					}
					catch (std::exception &e)
					{
						std::cerr << e.what() << std::endl;
						response.errorPage(GatewayTimeout);
					}
				}
				else if (loc.getAllowUpload() == false) {
					return response.errorPage(Forbidden);
				}
				else {
					std::string uploadDir;
					try {
						checkUploadDir(method, server, loc, uploadDir);
					} catch (std::exception &e) {
						std::cerr << e.what() << std::endl;
						return response.errorPage(BadRequest); // TO CHANGE ???
					}
					size_t boundaryPos = response.request.headers["Content-Type"].find("boundary=");
					if (boundaryPos == std::string::npos)
						return response.errorPage(BadRequest);
					std::string boundary = "--" + response.request.headers["Content-Type"].substr(boundaryPos + 9);
					std::string boundaryEnd = boundary + "--";
					if (boundary.length() >= BODY_BUFFER_SIZE)
					{
						webserv::logger::error("[multipart] BODY_BUFFER_SIZE too large");
						return response.errorPage(PayloadTooLarge);
					}
					std::ofstream output; // TO CREATE UPLOAD FILE OBJECT
					std::string tmpBuf; // TO READ BUFFER EASILY THAN A CHAR*
					size_t readSize = 0;
					std::vector<size_t> readSizeSave; // TO SAVE readSize after each READ, USE IT TO CHUNCK DATA
					char buffer[BODY_BUFFER_SIZE];
					std::string filename;
					readSize = response.request.readBody(buffer, BODY_BUFFER_SIZE);
					if (readSize <= 0)
					{
						webserv::logger::error("[multipart] Should have readSize > 0");
						return response.errorPage(BadRequest);
					}
					buffer[readSize] = '\0';
					tmpBuf.append(buffer, readSize);
					memset(buffer, '\0', sizeof(buffer)); // REINIT buffer FOR THE NEXT ITERATION
					readSizeSave.push_back(readSize);
					while (readSize > 0)
					{
						size_t  posBoundaryStart = tmpBuf.find(boundary); // NEXT BOUNDARY START POSITION
						size_t  posBodyEnd = tmpBuf.find(boundaryEnd); // END BODY POSITION
						if (posBoundaryStart != std::string::npos)
						{
							if (output.is_open() == true)
							{
								output.write(tmpBuf.c_str(), posBoundaryStart - 2); // EXCLUDE "\r\n" AFTER LAST CHAR OF THE FILE
								output.close();
								readSizeSave.clear();
								webserv::logger::debug("[multipart] 0* file closed: " + filename);
							}
							// SHIFT buf TO START OF BOUNDARY
							shiftBoundaryBuf(tmpBuf, readSize, posBoundaryStart, posBoundaryStart, posBodyEnd);
							// END OF THE MULTIPART BODY
							if (posBoundaryStart == posBodyEnd && posBoundaryStart != std::string::npos && posBoundaryStart == 0)
							{
								webserv::logger::debug("[multipart] End of Multipart Body");
								if (output.is_open() == true)
								{
									webserv::logger::debug("[multipart] 1* file closed: " + filename);
									output.close();
								}
								while (readSize > 0)
									readSize = response.request.readBody(buffer, BODY_BUFFER_SIZE);
								continue ;
							}
							// TO SHIFT AFTER "\r\n" OF THE BOUNDARY
							shiftBoundaryBuf(tmpBuf, readSize, boundary.length() + 2, posBoundaryStart, posBodyEnd);
							// CASE WHERE CONTENT-DISPOSITION IS SPLITTED BETWEEN TWO BUFFER
							// SEARCH FOR THE CARRIAGE RETURN AT THE END OF CONTENT-DISPOSITION
							// CHECK IF CONTENT-DISPOSITION IS FOUND
							// OTHERWISE, READ AGAIN TO HAVE IT
							size_t  posContentDisposition = getPosContentPart(response, tmpBuf, readSizeSave, readSize, "\r\n");
							if (posContentDisposition == std::string::npos)
							{
								webserv::logger::error("[multipart] No content Disposition");
								if (output.is_open() == true)
								{
									webserv::logger::debug("[multipart] 2* file closed: " + filename);
									output.close();
								}
								return response.errorPage(BadRequest);
							}
							// CREATE A TEMPORARY STRING TO ONLY HAVE THE FULL LINE OF CONTENT-DISPOSITION
							filename = tmpBuf.substr(0, posContentDisposition);
							// CHECK THE TYPE OF THE BOUNDARY
							size_t posFileName = filename.find("; filename=");
							if (posFileName != std::string::npos)
							{
								filename = filename.erase(0, posFileName + 12); // ERASE EVERY CHAR UNTIL TO HAVE THE START OF THE FILENAME VALUE
								filename = filename.erase(filename.find_last_of("\""));  // ERASE EVERY CHAR FROM THE LAST \" TO THE END OF THE STRING
								webserv::logger::debug("[multipart] new file : " + filename);
								if (filename.empty() == true)
								{
										webserv::logger::error("[multipart] filename is empty");
										return response.errorPage(BadRequest);
								}
								output.open((uploadDir + "/" + filename).c_str(), std::ios::out | std::ios::binary | std::ios::trunc);
								if (!output.good())
									return response.errorPage(Forbidden);
							}
							// CASE WHERE CONTENT-TYPE IS SPLITTED BETWEEN TWO BUFFER
							// SEARCH FOR THE CARRIAGE RETURN AT THE END OF CONTENT-TYPE
							// CONTENT-TYPE DOES NOT EXIST FOR BOUNDARY DIFFERENT THAN FILENAME
							// BUT THERE IS ALWAYS "\r\n\r\n" BETWEEN CONTENT-xxxxx AND THE DATA
							// CHECK IF CONTENT-TYPE IS FOUND
							// OTHERWISE, READ AGAIN TO HAVE IT
							size_t  posContentType = getPosContentPart(response, tmpBuf, readSizeSave, readSize, "\r\n\r\n");
							if (posContentType == std::string::npos)
							{
								webserv::logger::error("[multipart] No content Type");
								if (output.is_open() == true)
								{
									webserv::logger::debug("[multipart] 3* file closed: " + filename);
									output.close();
								}
								return response.errorPage(BadRequest);
							}
							// TO SHIFT AFTER "\r\n\r\n" OF THE CONTENT-TYPE TO HAVE A BUF STARTING BY THE DATA
							shiftBoundaryBuf(tmpBuf, readSize, posContentType + 4, posBoundaryStart, posBodyEnd);
						}
						else
						{
							if (readSizeSave.size() >= 3) // CHUNCK DATA AND ENSURE LARGE FILES UPLOAD
							{
								if (output.is_open() == true)
								{
									output.write(tmpBuf.c_str(), readSizeSave[0]); // WRITE THE FIRST readSize INTO THE FILE
									tmpBuf.erase(0, readSizeSave[0]); // DELETE THE tmpBuf PART ACCORDING TO THE FIRST readSize
									readSizeSave.erase(readSizeSave.begin()); // DELETE THE FIRST readSize
								}
							}
							readSize = response.request.readBody(buffer, BODY_BUFFER_SIZE);
							buffer[readSize] = '\0';
							tmpBuf.append(buffer, readSize);
							memset(buffer, '\0', sizeof(buffer)); // REINIT buffer FOR THE NEXT ITERATION
							if (readSize > 0)
								readSizeSave.push_back(readSize);
						}
					}
					return response.errorPage(Created);
				}
			}
		} catch (webserv::exceptions::http::FileNotFound &e) {
			response.errorPage(NotFound);
		} catch (webserv::exceptions::http::FileAccessDenied &e) {
			response.errorPage(Forbidden);
		}
	}
	catch (std::exception &e)
	{
		std::cerr << e.what() << std::endl;
		return response.errorPage(InternalServerError);
	}

}

std::string getArg(int argc, char **argv)
{
	if (argc < 2)
		return "srcs/config/defaultConfig";
	else
		return argv[1];
}

int main(int argc, char **argv, char **envp)
{
	if (argc > 2)
	{
		std::cerr << "Usage: " << argv[0] << " <config_file>" << std::endl;
		return (1);
	}

	std::string arg = getArg(argc, argv);
	try
	{
		webserv::config::ConfigFile config(arg);
		webserv::logger::setLevel(webserv::logger::LogLevel::Debug);
		// webserv::logger::setLevel(webserv::logger::LogLevel::Info);
		webserv::logger::debug("Preparing signal handlers (SIGINT)");

		struct sigaction action;
		action.sa_sigaction = gracefulExit;
		sigemptyset(&action.sa_mask);
		action.sa_flags = SA_SIGINFO;
		sigaction(SIGINT, &action, NULL);

		// Get all servers (ip, port) to listen on
		std::set<std::pair<std::string, int> > addedServers; // workaround to not change all other functions
		std::vector<std::pair<std::string, int> > servers;
		for (webserv::config::ConfigFile::map_type::const_iterator it = config.servers.begin(); it != config.servers.end(); ++it)
		{
			std::pair<std::string, int> serverData(it->second.getIP(), it->second.getPort());
			if (addedServers.find(serverData) != addedServers.end())
				continue;
			addedServers.insert(serverData);
			servers.push_back(serverData);
		}
		webserv::io::Socket socket(servers);
		while (running)
		{
			try
			{
				socket.loop(onConnection, envp, &config);
			}
			catch (webserv::exceptions::io::SocketInterrupted &e)
			{
				webserv::logger::debug("[socket] Socket interrupted");
			}
			catch (webserv::exceptions::io::SocketUnknownError &e)
			{
				webserv::logger::error("[socket] An unknown socket error occured");
				std::cerr << e.what() << std::endl;
			}
			catch (std::exception &e)
			{
				webserv::logger::error("[socket] An unknown error occured");
				std::cerr << e.what() << std::endl;
			}
		}
		socket.kill();
		webserv::logger::info("Goodbye !");
	}
	catch (std::exception &e)
	{
		std::cerr << e.what() << std::endl;
		return 1;
	}
	return 0;
}
