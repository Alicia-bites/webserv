#include <string>

#if defined(__linux__) || defined(__APPLE__)
	# define PATH_SEPARATOR '/'
#elif defined(_WIN32)
	# define PATH_SEPARATOR '\\'
#endif

/**
 * @brief Joins smartly two paths (using the correct separator for the OS)
 * 
 * @param p1 First part of the path to join
 * @param p2 Second part of the path to join
 * @return std::string Joined path
 */
namespace webserv {
	namespace utils {
		std::string path_join(std::string const &p1, std::string const &p2, char separator = PATH_SEPARATOR) {
			std::string output = p1;
			// Append separator if none is present
			if (output[output.size() - 1] != separator && p2[0] != separator)
				output += separator;
			// Concatenate paths
			output += p2;
			return output;
		}
	}
}
