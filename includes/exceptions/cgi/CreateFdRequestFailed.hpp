#pragma once
#include <exception>

namespace webserv {
	namespace exceptions {
		namespace cgi {
			class CreateFdRequestFailed: public std::exception {
				public:
					virtual const char* what() const throw() {
						return "Impossible to create a file descriptor to store the body request string";
					}
			};
		}
	}
}
