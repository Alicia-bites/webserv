#pragma once

#include <string>
#include <unistd.h>
#include <vector>

#if defined(__linux__) || defined(__APPLE__)
	# define PATH_SEPARATOR '/'
#elif defined(_WIN32)
	# define PATH_SEPARATOR '\\'
#endif

namespace webserv {
	namespace utils {
		namespace AccessFlags {
			enum AccessFlags {
				READABLE = R_OK,
				WRITABLE = W_OK,
				EXECUTABLE = X_OK,
				FILE = F_OK
			};
		}
	}
}

namespace webserv {
	namespace utils {
		unsigned int 	atoul(const char *s);
		std::string		ultoh(unsigned long n);
		std::string 	itoa(int i);
		std::string 	path_join(std::string const &p1, std::string const &p2, char separator = PATH_SEPARATOR);
		std::string		basename(const std::string &ref, char separator = PATH_SEPARATOR);
		bool			is(int flag, const std::string &ref);
		std::vector<std::string> split( const std::string& , char );
		void		replaceChr( std::string &, int, int );
	}
}
