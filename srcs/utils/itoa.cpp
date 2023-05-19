#include <string>
#include <algorithm>

namespace webserv {
	namespace utils {
		std::string itoa(int i) {
			std::string s;
			while (i) {
				s += (i % 10) + '0';
				i /= 10;
			}
			std::reverse(s.begin(), s.end());
			return s;
		}
	}
}
