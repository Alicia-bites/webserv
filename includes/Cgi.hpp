#pragma once

#include <cstring>
#include <fcntl.h>
#include <iostream>
#include <libgen.h>
#include <map>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <sstream>
#include <sys/wait.h>
#include <unistd.h>
#include <vector>
#include <poll.h>
#include "exceptions.hpp"
#include "logger.hpp"
#include "MetaVariable.hpp"
#include "utils.hpp"
#include "Response.hpp"
#include "constant.hpp"
#include "colors.hpp"

#define DFT_CGI_CMD "./resources/42_tester/cgi/ubuntu_cgi_tester"
#define CRLF "\r\n"
#define HEADER_END "\r\n\r\n"
#define HEADER_LINE_SEP ": "

namespace webserv {
	namespace io {
		class Cgi {
			/**
			 * @brief Cgi class
			 * 
			 * Constructor 1
			 * @param XXX XXX
			 * 
			 * @todo XXX
			 * 
			 * @note XXX
			 * @version 1.0.0
			 */
			private:
				// MEMBERS
				MetaVariable						_metaVariables;
				std::string							_contentPath;
				char								**_argv;
				bool								_argsAllocated;
				bool								_executed;
                bool								_outputRead;
				pid_t								_childPid;
				std::string							_body;
				bool								_headerRead;
				webserv::http::Response				&_response;

				// METHOD
				void		 						_createPipeFd( void );
				int			 						_exec();
				std::string	 						_path( const std::string & );
				int			 						_run( void );
				void								_forwardPostBody( void );
				void								parseHeader( char *, size_t, ssize_t & );
				void								forwardBody(char *, size_t, ssize_t & );
				void								showMetaVariables( void );


			public:
				// CONSTRUCTOR
				Cgi( webserv::http::Response &response, const std::string &cgiCmd, const std::string &contentPath );

				// DESTRUCTOR
				~Cgi( void );

				// MEMBERS				
				std::string							_cgiCmd; // BINARY CGI NAME
				int									_pfdIn[2]; // CGI PROGRAM PIPE FILE DESCRIPTOR
				int									_pfdOut[2]; // CGI PROGRAM PIPE FILE DESCRIPTOR
				std::map<std::string, std::string> 	headers; // CGI output headers

				// METHOD
				ssize_t      						readBody(char *dest, size_t n);
				bool		 						eof( void );
		};
	}
}
