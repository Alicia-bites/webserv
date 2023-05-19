#include <string>

#if defined(__linux__) || defined(__APPLE__)
	# define PATH_SEPARATOR '/'
#elif defined(_WIN32)
	# define PATH_SEPARATOR '\\'
#endif

/**
 * @brief Returns the basename of a path
 * 
 * @param ref The path to get the basename from
 * @param separator The path separator to use
 * @return std::string The basename of the given path
 */
namespace webserv {
    namespace utils {
        std::string basename(const std::string &ref, char separator = PATH_SEPARATOR) {
            std::string output = ref;
            // Remove trailing separator
            if (output[output.size() - 1] == separator)
                output.erase(output.size() - 1);
            // Find last separator
            size_t pos = output.find_last_of(separator);
            if (pos == std::string::npos)
                return output;
            // Return basename
            return output.substr(pos + 1);
        }
    }
}
