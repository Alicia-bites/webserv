#pragma once
#include <exception>

namespace webserv {
	namespace exceptions {
		namespace cgi {
			class WriteIntoFdFailed: public std::exception {
				public:
					virtual const char* what() const throw() {
						return "Impossible to write into fd";
					}
			};
		}
	}
}
