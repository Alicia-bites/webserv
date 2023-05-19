#pragma once
#include <exception>

namespace webserv {
	namespace exceptions {
		namespace cgi {
			/**
			 * @brief Exception thrown when pipe issue
			 */
			class PipeFailed: public std::exception {
				public:
					virtual const char* what() const throw() {
						return "Pipe failed";
					}
			};
		}
	}
}
