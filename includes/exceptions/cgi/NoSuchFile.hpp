#pragma once
#include <exception>

namespace webserv {
	namespace exceptions {
		namespace cgi {
			/**
			 * @bried Exception thrown when no such file
			 */
			class NoSuchFile: public std::exception {
				public:
					virtual const char* what() const throw() {
						return "No executable found for CGI subprocess";
					}
			};
		}
	}
}
