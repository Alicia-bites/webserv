#pragma once
#include <exception>

namespace webserv {
	namespace exceptions {
		namespace cgi {
			class CloseFailedChild: public std::exception {
				public:
					virtual const char* what() const throw() {
						return "Close failed on child";
					}
			};
		}
	}
}
