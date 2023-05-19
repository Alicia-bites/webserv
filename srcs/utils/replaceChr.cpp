#include "utils.hpp"

namespace webserv {
	namespace utils {
		void	replaceChr( std::string &str, int cSearch, int cReplace )
		{
			for (std::string::iterator it = str.begin(); it != str.end(); ++it)
			{
				if (*it == cSearch)
					*it = cReplace;
			}
		}
	}
}
