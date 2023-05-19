#pragma once

#include "configFile.hpp"
#include <string>
#include <map>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>

# define BUFFER_SIZE 4096
# define HTTP_QUERY_SEP "?"
# define HTTP_ANCHOR "#"

namespace webserv {
    namespace http {
        class Request {
            /**
             * @brief Request class, represents a HTTP request
             * 
             * Constructor 1
             * @param buffer Buffer containing the request headers
             * 
             * Constructor 2
             * @param method HTTP method
             * @param path Requested path
             * @param protocol HTTP protocol
             * @param headers HTTP headers
             * 
             * @note Buffer type might be changed to something else later
             * @version 1.0.0
             */
            public:
                std::string method;
                std::string path;
				std::string virtualPath;
                std::string uri;
                std::string query;
                std::string protocol;
                bool        invalid;
                char        **envp;
                std::string address;
                int         port;
                std::map<std::string, std::string> headers;
                webserv::config::ConfigFile *config;

                // Helper methods
                std::string getHeader(std::string const &key) const;
                size_t      readBody(char *dest, size_t n);
                std::string	getUri( void );
                std::string	getQuery( void );

                void setEnv(char **envp);

                Request(int socketFd, webserv::config::ConfigFile *config);
                Request(std::string method,
                    std::string path,
                    std::string protocol,
                    std::map<std::string, std::string> headers
                );
                // Constructor used for unit tests
                Request(std::string const &buffer);

                ~Request();

            private:
                int         _socketFd;
                char        _buffer[BUFFER_SIZE];
                size_t      _bodyReadBytes;
                size_t      _overReadBytes;
                bool        _bodyRead;
        };
    }
}

void parseHeaderLine(std::string, std::map<std::string, std::string> &);
