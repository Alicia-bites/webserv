#include "Request.hpp"
#include "exceptions.hpp"
#include "encoding.hpp"
#include <sstream>
#include <sys/types.h>
#include <sys/socket.h>
#include <cstring>
#include <iostream>

using namespace webserv;
using namespace webserv::http;

const std::string SUPPORTED_METHODS[] = {
    "GET",
    "POST",
    "DELETE",
};

/**
 * @brief Parse a header line and add it to the headers map
 * 
 * @param line The current line to parse
 * @param headers A reference to the headers map
 * @throw InvalidHeaderFormat if the line is badly formatted
 */
void parseHeaderLine(std::string line, std::map<std::string, std::string> &headers) {
    std::string key, value;

    // Find the first ':' to separate the key from the value, if there is none, throw an exception
    size_t pos = line.find(":");
    if (pos == std::string::npos) // ':' isn't found, the format is not RFC compliant
        throw exceptions::http::InvalidHeaderFormat();

    // Get the key and the value
    key = line.substr(0, pos);

    value = line.substr(pos + 2);

    if (key.empty()) // If the key is empty, the format is not RFC compliant
        throw exceptions::http::InvalidHeaderFormat();
    
    // ACCORDING TO NGINX RESULT DUPLICATE HEADER FIELD OVERWRITTE THE PREVIOUS VALUE
    // Check if the key is already in the map, if it is, throw an exception
    //if (headers.find(key) != headers.end())
    //    throw exceptions::http::DuplicateHeader();

    // Add the key and the value to the map
    headers[key] = value;
}

/**
 * @brief Parse the request line and set the method, path and protocol attributes of the Request object
 * 
 * @param line The current line to parse
 * @throw InvalidRequestLine if the line is badly formatted
 */
void parseRequestLine(std::string line, std::string &method, std::string &path, std::string &protocol) {
    // These three values are separated by a space, so we're using a stream to parse them (cool trick indeed)
    std::istringstream stream(line);
    stream >> method >> path >> protocol;

    // If the stream is not empty, it means that there is more than 3 values, so the line is badly formatted
    if (!stream.eof())
    {
        // Reset the values
        method.clear();
        path.clear();
        protocol.clear();
        throw exceptions::http::InvalidRequestLine();
    }

    // Check if any of the values is empty, if so, the line is badly formatted
    if (method.empty() || path.empty() || protocol.empty())
        throw exceptions::http::InvalidRequestLine();

    // Check if method is supported, if not, throw an exception
    for (size_t i = 0; i < sizeof(SUPPORTED_METHODS) / sizeof(SUPPORTED_METHODS[0]); i++) {
        if (method == SUPPORTED_METHODS[i])
            return ;
    }
    throw exceptions::http::InvalidMethod();
}

Request::Request(int socketFd, webserv::config::ConfigFile *config): config(config), _socketFd(socketFd) {
    invalid = false;
    _bodyRead = false;
    std::string requestData;
    char readBuffer[BUFFER_SIZE] = {0};
    bool hasBody = false;
    size_t readSize = 0;

    while ((readSize = recv(_socketFd, readBuffer, BUFFER_SIZE, 0)) > 0) {
        // Determine if the request is a GET or a POST request
        if (requestData.size() == 0) // first read
        {
            requestData.append(readBuffer, readSize);
            hasBody = requestData.rfind("POST", 0) == 0; // if the request starts with "POST", it has a body
        }
        else
            requestData.append(readBuffer, readSize);
        if (requestData.rfind("\r\n\r\n") != std::string::npos)
            break ;
    }
    // Parse the request line
    try {
        std::istringstream stream(requestData);
        std::string line = requestData.substr(0, requestData.find("\r\n"));
        parseRequestLine(line, method, path, protocol);
        // Remove the request line from the buffer
        requestData.erase(0, requestData.find("\r\n") + 2);
        path = webserv::encoding::decodeURI(path);
	uri = getUri();
	query = getQuery();
    } catch (std::exception &e) {
        invalid = true;
        return ;
    }
    try {
        std::istringstream stream(requestData);
        std::string line;
        while (std::getline(stream, line)) {
            // Remove the line from the buffer
            requestData.erase(0, requestData.find("\r\n") + 2);
            // If there is a carriage return, remove it
            if (line[line.size() - 1] == '\r')
                line.erase(line.size() - 1);
            // Check if the line is empty, if so, we're done parsing the headers
            if (line.empty() || line == "\n")
                break ;
            // Other lines are just Key: Value headers
            parseHeaderLine(line, headers);
        }
    } catch (std::exception &e) {
        invalid = true;
        return ;
    }
    if (hasBody) {
        _overReadBytes = requestData.size();
        _bodyReadBytes = _overReadBytes;
        // Keep track of how many bytes we've over-read because if the request is a POST
        // request, the Content-Length header will tell us how many bytes we need to read in total

        // If we're here, the request is valid and in our buffer there's nothing
        // or over-read data (which is the body, we don't really care about it here)
        if (_overReadBytes == 0) // Nothing to do, just return to avoid undefined behaviors
            return ;
        strncpy(_buffer, requestData.c_str(), _overReadBytes);
        memset(_buffer + _overReadBytes, 0, BUFFER_SIZE - _overReadBytes);
    } else {
        _overReadBytes = 0;
        _bodyReadBytes = 0;
    }
    // use getaddrinfo to fill this->clientAddress
    struct sockaddr_in addr;
    socklen_t addr_size = sizeof(struct sockaddr_in);
    getpeername(_socketFd, (struct sockaddr *)&addr, &addr_size);
    char ipstr[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &(addr.sin_addr), ipstr, INET_ADDRSTRLEN);
    address = ipstr;
}

/**
 * @brief Construct a new Request object from a buffer (unit tests only)
 * 
 * @param buffer Buffer containing the request
 */
Request::Request(std::string const &buffer) {
    invalid = false;
    _bodyRead = false;
    std::string requestData = buffer;
    bool hasBody = false;

    // Determine if the request is a GET or a POST request
    if (requestData.size() == 0) // first read
    {
        hasBody = requestData.rfind("POST", 0) == 0; // if the request starts with "POST", it has a body
    }
    else
        hasBody = requestData.rfind("POST", 0) == 0; // if the request starts with "POST", it has a body
    // Parse the request line
    std::string line;
    {
        std::istringstream stream(requestData);
        line = requestData.substr(0, requestData.find("\r\n"));
        parseRequestLine(line, method, path, protocol);
        // Remove the request line from the buffer
        requestData.erase(0, requestData.find("\r\n") + 2);
        path = webserv::encoding::decodeURI(path);
    }
    {
        std::istringstream stream(requestData);
        while (std::getline(stream, line)) {
            // Remove the line from the buffer
            requestData.erase(0, requestData.find("\r\n") + 2);
            // If there is a carriage return, remove it
            if (line[line.size() - 1] == '\r')
                line.erase(line.size() - 1);
            // Check if the line is empty, if so, we're done parsing the headers
            if (line.empty() || line == "\n")
                break ;
            // Other lines are just Key: Value headers
            parseHeaderLine(line, headers);
        }
    }
    if (hasBody) {
        _overReadBytes = requestData.size();
        _bodyReadBytes = _overReadBytes;
        // Keep track of how many bytes we've over-read because if the request is a POST
        // request, the Content-Length header will tell us how many bytes we need to read in total

        // If we're here, the request is valid and in our buffer there's nothing
        // or over-read data (which is the body, we don't really care about it here)
        if (_overReadBytes == 0) // Nothing to do, just return to avoid undefined behaviors
            return ;
        strncpy(_buffer, requestData.c_str(), _overReadBytes);
        memset(_buffer + _overReadBytes, 0, BUFFER_SIZE - _overReadBytes);
    } else {
        _overReadBytes = 0;
        _bodyReadBytes = 0;
    }
}

/**
 * @brief Get a header value from the headers map
 * 
 * @param key The key to search
 * @return std::string The value of the header, or an empty string if the header isn't defined
 */
std::string Request::getHeader(std::string const &key) const
{
    std::map<std::string, std::string>::const_iterator it = this->headers.find(key);
    return it == this->headers.end() ? "" : it->second;
}

/**
 * @brief Reads n bytes from the socket and stores them in dest
 * 
 * @param dest Destination buffer, must be at least n bytes long
 * @param n Number of bytes to read
 * @return size_t The number of bytes read, throws an exception if the socket is closed
 * 
 * @throw PipeClosed if recv fails
 * 
 */
size_t Request::readBody(char *dest, size_t n)
{
    if (_bodyRead)
        return 0;
    size_t totalLength = 0;
    if (headers.find("Content-Length") != headers.end())
    {
        std::istringstream stream(headers["Content-Length"]);
        stream >> totalLength;
    }
    if (totalLength == 0) // No Content-Length header, we can't read anything
    {
        _bodyRead = true;
        return 0;
    }
    // If we've already read some bytes, copy them to the destination buffer
    if (_overReadBytes != 0) {
        // Prevent buffer overflows
        size_t toCopy = _overReadBytes > n ? n : _overReadBytes;
        _overReadBytes -= toCopy;

        // Copy the bytes we've already read and return it for safety
        strncpy(dest, _buffer, toCopy);
        memmove(_buffer, _buffer + toCopy, toCopy);
        return toCopy;
    }
    // If we've read all the bytes, we're done
    if (_bodyReadBytes == totalLength) {
        _bodyRead = true;
        return 0;
    }
    // Read the rest of the bytes from the socket
    ssize_t readSize = recv(_socketFd, dest + _overReadBytes, n, 0);
    if (readSize == -1) // Socket closed
        throw exceptions::io::PipeClosed();
    if (_bodyReadBytes == totalLength) // We've read all the bytess
        _bodyRead = true;
    _bodyReadBytes += readSize;
    return readSize;
}

void Request::setEnv(char **envp)                       { this->envp = envp; }
Request::Request(std::string method, std::string path, std::string protocol, std::map<std::string, std::string> headers):
    // config(webserv::config::ConfigFile()),
    method(method),
    path(path),
    protocol(protocol),
    headers(headers)
{}

/**
 * @brief get uri part from the path member without query part
 * 
 * @param c The query character
 */
std::string	Request::getUri( void )
{
	size_t	npos = path.find(HTTP_QUERY_SEP);
	if (npos == std::string::npos)
		return (path.substr(0));
	else
		return (path.substr(0, npos));
}


/**
 * @brief get query part from the path member
 * 
 * @param c The query character
 */
std::string	Request::getQuery( void )
{
	size_t	npos = path.find(HTTP_QUERY_SEP);
	size_t	nposAnchor = path.find(HTTP_ANCHOR);
	if (npos == std::string::npos)
		return ("");
	else
	{
		if (nposAnchor == std::string::npos)
			return (path.substr(npos + 1));
		else
			return (path.substr(npos + 1, nposAnchor));
	}
}

Request::~Request(void) {}
