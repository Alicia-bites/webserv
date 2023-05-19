#pragma once
#include <exception>

namespace webserv {
	namespace exceptions {
		namespace cgi {
			class CloseFailedParent: public std::exception {
				public:
					virtual const char* what() const throw() {
						return "Close failed on parent";
					}
			};
		}
	}
}
