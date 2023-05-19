#pragma once
/* NOTE:
 * META-VARIABLE remote_ident IS NOT MANDATORY
 * SEE https://www.rfc-editor.org/rfc/rfc3875#section-4.1.10
 * SOURCE:
 * - https://www.rfc-editor.org/rfc/rfc3875#section-4.1
 */

#include <ctype.h>
#include <iostream>
#include <map>
#include <cstdlib>
#include <string>
#include "Request.hpp"
#include "exceptions.hpp"
#include <algorithm>

/*** META-VARIABLE NAME ***/
#define AUTH_TYPE "AUTH_TYPE"
#define CONTENT_LENGTH "CONTENT_LENGTH"
#define CONTENT_TYPE "CONTENT_TYPE"
#define GATEWAY_INTERFACE "GATEWAY_INTERFACE"
#define PATH_INFO "PATH_INFO"
#define PATH_TRANSLATED "PATH_TRANSLATED"
#define QUERY_STRING "QUERY_STRING"
#define REMOTE_ADDR "REMOTE_ADDR"
#define REMOTE_HOST "REMOTE_HOST"
#define REMOTE_IDENT "REMOTE_IDENT"
#define REMOTE_USER "REMOTE_USER"
#define REQUEST_METHOD "REQUEST_METHOD"
#define SCRIPT_NAME "SCRIPT_NAME"
#define SERVER_NAME "SERVER_NAME"
#define SERVER_PORT "SERVER_PORT"
#define SERVER_PROTOCOL "SERVER_PROTOCOL"
#define SERVER_SOFTWARE "SERVER_SOFTWARE"
#define REDIRECT_STATUS "REDIRECT_STATUS"

/*** META-VARIABLED DEFAULT VALUE***/

// Vars set by the CGI class
#define DFT_VAL_PATH_INFO ""
#define DFT_VAL_PATH_TRANS ""
#define DFT_VAL_REQ_METH "GET"
#define DFT_VAL_SRV_PROTO "HTTP/1.1"

// No changes needed for these vars (constants)
#define DFT_VAL_SCHEME "http"
#define DFT_VAL_SRV_SOFT "serveurtoile/1.0"
#define DFT_VAL_CGI "CGI/1.1"

// TODO: Implement these vars
#define DFT_VAL_REDIRECT_STATUS "200"
#define DFT_VAL_AUTH_TYPE ""
#define DFT_VAL_CONT_LEN ""
#define DFT_VAL_CONT_TYPE ""
#define DFT_VAL_EXT_VAR_NAME "TBD"
#define DFT_VAL_PROTO_VAR_NAME "HTTP/1.1_TBD"
#define DFT_VAL_QUERY_STR ""
#define DFT_VAL_REMOTE_ADDR "127.0.0.1"
#define DFT_VAL_REMOTE_HOST "localhost"
#define DFT_VAL_REMOTE_USER "user"
#define DFT_VAL_SCRIPT_NAME ""
#define DFT_VAL_SRV_NAME "localhost"
#define DFT_VAL_SRV_PORT "8080"

#define HTTP_KEY_SEP '-'
#define META_KEY_SEP '_'
#define META_HTTP_PREFIX "HTTP_"

namespace webserv {
	namespace io {
		class MetaVariable {
			/**
			 * @brief MetaVariable class, represents meta-variable used for CGI Script
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
				unsigned int	_nbVars; // Used to free at destruction
				bool			_isInitialized; // Check if _vars is malloc'd
				char			**_vars;

				// METHOD
				void	setDefaultVars( void );
				void	setHttpVars( webserv::http::Request const & );

			public:
				// MEMBERS
				std::map<std::string, std::string> var;

				// METHOD
				char	**getVars( void );

				// CONSTRUCTOR
				MetaVariable( webserv::http::Request const &);

				// DESTRUCTOR 
				~MetaVariable( void );

				// XXX:
				// TODO: Create a method to convert a string to char**??
				// TODO: Create a method to add or update an environment variable
				// TODO: Create a method to add or update all environment variables
		};
	}
}

std::ostream	&operator << ( std::ostream &, webserv::io::MetaVariable const & );
