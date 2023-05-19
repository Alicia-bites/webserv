#include <string>
#include <sstream>

namespace webserv {
	namespace utils {
        /**
         * @brief Converts an unsigned long to its base16 representation
         * 
         * @param n Number to convert
         * @return std::string Output string
         */
		std::string ultoh(unsigned long n) {
            std::stringstream sstream;
            sstream << std::hex << n;
            std::string result = sstream.str();
            return result;
        }
	}
}
