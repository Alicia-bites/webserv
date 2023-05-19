#pragma once

#include <string>
#include <vector>
#include <fstream>
#include "Request.hpp"

namespace webserv {
    namespace http {
        class Response {
            /**
             * @brief Response class, represents a HTTP response
             * 
             * @version 1.0.0
            */
            public:
				// Members
                webserv::http::Request request;
				unsigned int statusCode;
                int fileDescriptor;
				std::map<std::string, std::string> responseHeaders;
                webserv::config::ConfigFile *config;

				// Methods
                void    directory(std::string const &path);
				void    file(std::string const &path);
                void    html(std::string const &html);
                void    errorPage(unsigned int code);
                void    cgi(std::string const & cgiCommand, std::string const & path);
                void    writeChunk(const std::string &);
                void    writeChunk(const void *, size_t);
                void    closeChunkedResponse(void);
                ssize_t clientSend(const void *buffer, size_t n, int additionalFlags = 0);

                // Helper methods
                std::string getHeader(std::string const &key);
                bool setHeader(std::string const &key, std::string const &value, bool overwrite = false);

				// Constructor
                Response(webserv::http::Request const &request, int responseFd, webserv::config::ConfigFile *config);

				// Destructor
				~Response();

            private:
                std::ifstream _file;     // File descriptor, if file method is used
                bool _fileOpened;        // Flag set for destructor (close file)
                unsigned long _fileSize; // Size of the file to be streamed
                bool _headersSent;        // Flag set when headers are sent

                void _writeHeaders();
                void _writeResponseLine();
        };
    }
}
