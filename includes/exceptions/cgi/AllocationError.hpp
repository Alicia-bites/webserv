#pragma once
#include <exception>

namespace webserv {
	namespace exceptions {
		namespace cgi {
			class AllocationError: public std::exception {
				public:
					virtual const char* what() const throw() {
						return "Allocation issue";
					}
			};
		}
	}
}
