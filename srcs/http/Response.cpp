#include "Response.hpp"
#include "encoding.hpp"
#include "Socket.hpp"
#include "Cgi.hpp"
#include "logger.hpp"
#include "codes.hpp"
#include "exceptions.hpp"
#include "utils.hpp"
#include "colors.hpp"
#include <iostream>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <ctime>
#include <algorithm>

using namespace webserv::http;

# define PIPE_BUFFER_SIZE 16384

std::string itoa(size_t i) {
	std::string s;
	while (i) {
		s += (i % 10) + '0';
		i /= 10;
	}
	std::reverse(s.begin(), s.end());
	return s;
}

/**
 * @brief Creates a Date header in RFC 1123 format
 * 
 * @note Believe it or not, we need an entire function for this because
 * the date format is very specific.
 * 
 * @return std::string Date header value
 */
std::string generateDateHeader(time_t now = time(0)) {
    tm	*ltm = localtime(&now);

	std::string output = "";
	// day-name
	switch (ltm->tm_wday) {
		case 0: output += "Sun, "; break;
		case 1: output += "Mon, "; break;
		case 2: output += "Tue, "; break;
		case 3: output += "Wed, "; break;
		case 4: output += "Thu, "; break;
		case 5: output += "Fri, "; break;
		case 6: output += "Sat, "; break;
	}
	// Day number
	output += itoa(ltm->tm_mday) + " ";
	// Month number
	output += itoa(ltm->tm_mon) + " ";
	// Year
	output += itoa(1900 + ltm->tm_year) + " ";
	// Hour
	output += itoa(ltm->tm_hour) + ":";
	// Minute
	output += itoa(ltm->tm_min) + ":";
	// Second
	output += itoa(ltm->tm_sec) + " ";

	// Timezone
	output += "GMT"; // TODO: Maybe properly handle timezones?
	return output;
}


Response::Response(Request const &request, int responseFd, webserv::config::ConfigFile *config) : request(request) {
	// By default, we set the status code to 200 (OK)
	this->statusCode = 200;
	this->fileDescriptor = responseFd;
	this->_fileOpened = false;
	this->_headersSent = false;
	this->config = config;
}

Response::~Response() {
	if (this->_fileOpened) {
		this->_file.close();
	}
	std::cerr << "[" << statusCode << "] @ " << request.address << " - " << request.method << " " << request.path << " ~ " << request.getHeader("User-Agent") << std::endl;
}

/**
 * @brief Dynamically generates an index (if enabled in config)
 * 
 * @param path Path to the directory
 * 
 * @note If the config doesn't allow directory listing, this method will send a 404 error page
 * @note Asserts that the path is a directory (undefined behavior if it isn't)
 * @note Behavior is undefined if any error occurs
 */
void Response::directory(std::string const &path) {
	// Set Content-Type, as we're sending a chunked response
	responseHeaders["Content-Type"] = "text/html";

	// Open directory to get files / folders inside
	DIR *dir = opendir(path.c_str());
	if (!dir) {
		webserv::logger::error("[response] Unable to open directory");
		return errorPage(403);
	}
	try {
		struct dirent *entry; // Will store data for each entry
		writeChunk("<html><head><title>Index of " + path.substr(2) + "</title></head><body><h1>Index of " + path.substr(1) + "</h1><hr><pre>"); // Send the HTML header
		if (request.path != "/") { // Not on root, so we add a link to the parent directory
			if (this->request.path.find_last_of('/') == 0) // If we're 1 level deep
				writeChunk("<a href=\"/\">../</a><br>");   // Go to root
			else // If we're more than 1 level deep
				writeChunk("<a href=\"" + this->request.path.substr(0, this->request.path.find_last_of('/')) + "\">../</a><br>");
		}
		// Loop through all entries
		while ((entry = readdir(dir))) {
			if (entry->d_name[0] == '.') // Ignore hidden files (maybe add a config option for this?)
				continue;
			std::string entryPath = webserv::utils::path_join(path, std::string(entry->d_name));
			std::string htmlPath = webserv::utils::path_join(this->request.path, std::string(entry->d_name));

			struct stat fileStat;
			if (stat(entryPath.c_str(), &fileStat) < 0) {
				webserv::logger::error("[response] Unable to stat file");
				continue ;
			}
			if (S_ISDIR(fileStat.st_mode) == 0) // Add a slash if it's a directory
				writeChunk("<a href=\"" + htmlPath + "\">" + entry->d_name + "</a><br>");
			else
				writeChunk("<a href=\"" + htmlPath + "\">" + entry->d_name + "/</a><br>");
		}
		closedir(dir);
		// Close all HTML tags
		writeChunk("</pre><hr></body></html>");
		// Close chunked reponse
		closeChunkedResponse();
	} catch (webserv::exceptions::io::PipeClosed &e) {
		closedir(dir);
		throw e; // Re-throw the exception
	}
}

/**
 * @brief Smartly sends a file to the client, setting headers if needed
 * 
 * @note If the file is a directory, it will send a directory listing (if enabled in config)
 * 
 * @param path Path to the file to stream
 * @throws webserv::exceptions::http::FileNotFound if the file doesn't exist
 * @throws webserv::exceptions::http::FileAccessDenied if the file can't be read
*/
void Response::file(std::string const &path) {
	// Check if the path is a directory
	struct stat fileStat;

	// checks if a file exists at the path specified by the string path, 
	// and retrieves some information about the file if it does exist. 
	if (stat(path.c_str(), &fileStat) != 0) {
		webserv::logger::error("[response] Unable to stat file");
		throw webserv::exceptions::http::FileNotFound();
	}

	// If path is indeed a directory, it calls the directory() function 
	// to handle the directory and returns from the current function.
	if (S_ISDIR(fileStat.st_mode)) {
		if (request.config->getServer(request.getHeader("Host"), request.port).getDirectoryIndex() == 1
			&& request.config->getServer(request.getHeader("Host"), request.port).getIndex().size() == 0)
			{
				this->directory(path);
			}
		return ;
	}

	// checks if the file at the path specified by the string path 
	// is readable by the current process. If the file is not readable, it throws an exception
	if (access(path.c_str(), R_OK) != 0)
		throw webserv::exceptions::http::FileAccessDenied();

	// Open the file
	this->_file.open(path.c_str(), std::ios::in | std::ios::binary);

	// Check if it was opened
	if (!this->_file.is_open()) {
		webserv::logger::error("[response] Unable to open file");
		throw webserv::exceptions::http::FileNotFound();
	}
	
	// Check for read permission
	if (!this->_file.good()) {
		webserv::logger::error("[response] Bad permissions");
		this->_file.close();
		throw webserv::exceptions::http::FileAccessDenied();
	}

	// Set the flag
	this->_fileOpened = true;

	// Get the file size
	this->_file.seekg(0, std::ios::end);
	this->_fileSize = this->_file.tellg();
	this->_file.seekg(0, std::ios::beg);

	// Set the Content-Length header to the file size
	this->responseHeaders["Content-Length"] = itoa(this->_fileSize);

	// Use stat to get the file's last modified date
	this->responseHeaders["Last-Modified"] = generateDateHeader(fileStat.st_mtime);

	// Check if content-type is already set
	if (this->responseHeaders.find("Content-Type") == this->responseHeaders.end())
		this->responseHeaders["Content-Type"] = webserv::encoding::mimeFromPath(path);

	// Send HTTP headers to client
	this->_writeResponseLine();
	this->_writeHeaders();

	// Read the file in chunks of PIPE_BUFFER_SIZE
	char buffer[PIPE_BUFFER_SIZE];
	webserv::logger::info("[response] Sending file");
	while (true) {
		this->_file.read(buffer, PIPE_BUFFER_SIZE);
		this->clientSend(buffer, this->_file.gcount());
		if (this->_file.gcount() == 0)
			break;
	}
	this->_file.close();
}

/**
 * @brief Buffers a text into the response buffer and sets headers automatically
 * 
 * @param html HTML source code to buffer
 */
void Response::html(std::string const &html) {
	this->responseHeaders["Content-Type"] = "text/html";
	this->responseHeaders["Content-Length"] = itoa(html.size());
	this->_writeResponseLine();
	this->_writeHeaders();
	this->clientSend(html.c_str(), html.size());
}

/**
 * @brief Generates an error page and sends it to the client (setting the status code)
 * 
 * @param code HTTP status code
*/
void Response::errorPage(unsigned int code) {
	// If a redirection is already set
	if (code >= 300 && code <= 399)
	{
		if (this->responseHeaders.find("Location") != this->responseHeaders.end())
		{
			this->statusCode = code;
			return this->html(webserv::http::codes::generateHTML(code));
		}
	}
	this->statusCode = code;
	webserv::config::Server server;
	try {
		server = config->getServer(this->request.getHeader("Host"), request.port);
	} catch (std::exception &e) {
		std::cerr << e.what() << std::endl;
		return this->html(webserv::http::codes::generateHTML(404));
	} 
	std::map<int, std::string> errorPage = server.getErrorPage();
	std::map<int, std::string>::const_iterator it = errorPage.begin();
	int code_i = (int)code;
	for (; it != errorPage.end(); ++it) {
		if (code_i == it->first) {
			std::string path = webserv::utils::path_join(server.getRoot(), it->second);
			if (access(path.c_str(), F_OK) == 0) {
				return this->file(path);
			} else {
				this->responseHeaders["Location"] = it->second;
				this->statusCode = 302;
				return this->html(webserv::http::codes::generateHTML(302));
			}
		}
	}
	this->html(webserv::http::codes::generateHTML(code));
}

void Response::cgi(std::string const & cgiCommand, std::string const & path) {
	webserv::io::Cgi cgi(
		*this,
		cgiCommand,
		path
	);
	char buffer[8192] = {0};
	ssize_t readSize = 0;
	bool firstRead = false;
	while (!cgi.eof())
	{
		readSize = cgi.readBody(buffer, 1024);
		if (!firstRead)
		{
			firstRead = true;
			responseHeaders = cgi.headers;
		}
		writeChunk(buffer, readSize);
	}
	closeChunkedResponse();
}

void Response::writeChunk(const std::string &buffer) {
	writeChunk(buffer.c_str(), buffer.size());
}

void Response::writeChunk(const void *buffer, size_t n) {
	if (!_headersSent) {
		responseHeaders["Transfer-Encoding"] = "chunked";
		this->_writeResponseLine();
		this->_writeHeaders();
		_headersSent = true;
	}
	{
		std::string length = utils::ultoh(n) + "\r\n";
		this->clientSend(length.c_str(), length.size());
	}
	this->clientSend(buffer, n);
	this->clientSend("\r\n", 2);
}

void Response::closeChunkedResponse(void) {
	this->clientSend("0\r\n\r\n", 5);
}

// Hanging fix : https://beej.us/guide/bgnet/html/#sendall
ssize_t Response::clientSend(const void *buffer, size_t n, int additionalFlags) {
	size_t total = 0;
	size_t remaining = n;
	if (additionalFlags == MSG_NOSIGNAL)
		additionalFlags = 0;
	while (total < n) {
		ssize_t bytesSent = send(this->fileDescriptor, (char *)buffer + total, remaining, MSG_NOSIGNAL | additionalFlags);
		if (bytesSent == -1)
			throw webserv::exceptions::io::PipeClosed();
		total += bytesSent;
		remaining -= bytesSent;
	}
	return total;
}

/**
 * @brief Get a header value from the headers map
 * 
 * @param key The key to search
 * @return std::string The value of the header, or an empty string if the header isn't defined
 */
std::string Response::getHeader(std::string const &key)
{
	std::map<std::string, std::string>::iterator it = this->responseHeaders.find(key);
	return it == this->responseHeaders.end() ? "" : it->second;
}

/**
 * @brief Set a header value in the headers map
 * 
 * @param key The key to write to
 * @param value The value to write to the key
 * @param overwrite Overwrite the value if the key is already set (slower, default: false)
 * @return true if the header was set, false otherwise (if already exists and overwrite is false)
 */
bool Response::setHeader(std::string const &key, std::string const &value, bool overwrite)
{
	if (!overwrite && this->responseHeaders.find(key) != this->responseHeaders.end())
		return false;
	this->responseHeaders[key] = value;
	return true;
}

/**
 * @brief Writes header to the socket's file descriptor (recv)
*/
void Response::_writeHeaders() {
	if (_headersSent)
		return ;
	std::map<std::string, std::string>::iterator it = this->responseHeaders.begin();
	for (size_t i = 0; i < this->responseHeaders.size(); i++) {
		std::string header = it->first + ": " + it->second + "\r\n";
		this->clientSend(header.c_str(), header.size());
		it++;
	}
	// https://www.w3.org/Protocols/rfc2616/rfc2616-sec14.html#sec14.18
	// https://developer.mozilla.org/fr/docs/Web/HTTP/Headers/Date
	// Overwrite the Date header if it was set by the user (it shouldn't be)
	// Check if the response code is 5xx, we mustn't send a Date header
	if (this->statusCode / 100 != 5) {
		std::string header = "Date: " + generateDateHeader() + "\r\n";
		this->clientSend(header.c_str(), header.size());
	}
	this->clientSend("Connection: close\r\n", 19);

	// https://developer.mozilla.org/en-US/docs/Web/HTTP/Headers/Server
	this->clientSend("Server: serveurtoile\r\n", 22);

	// https://developer.mozilla.org/en-US/docs/Web/HTTP/Headers/Accept-Ranges
	this->clientSend("Accept-Ranges: bytes\r\n", 22);
	this->clientSend("\r\n", 2);
	_headersSent = true;
}

/**
 * @brief Writes the response line to the socket's file descriptor (recv)
*/
void Response::_writeResponseLine() {
	if (_headersSent)
		return ;
	std::string responseLine = "HTTP/1.1 " + itoa(this->statusCode) + "\r\n";
	this->clientSend(responseLine.c_str(), responseLine.size());
}
