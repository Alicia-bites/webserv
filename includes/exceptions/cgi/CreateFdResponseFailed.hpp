#pragma once
#include <exception>

namespace webserv {
	namespace exceptions {
		namespace cgi {
			class CreateFdResponseFailed: public std::exception {
				public:
					virtual const char* what() const throw() {
						return "Impossible to create a file descriptor to store the response from CGI program";
					}
			};
		}
	}
}
