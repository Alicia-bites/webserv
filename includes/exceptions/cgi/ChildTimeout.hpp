#pragma once
#include <exception>

namespace webserv {
	namespace exceptions {
		namespace cgi {
			class ChildTimeout: public std::exception {
				public:
					virtual const char* what() const throw() {
						return "CGI child process timed-out.";
					}
			};
		}
	}
}
