#pragma once

#include <string>
#include <map>

namespace webserv {
	namespace encoding {
		std::string decodeURI(std::string const &);
		std::string mimeFromExtension(const std::string &);
		std::string mimeFromPath(const std::string &);
		std::string extensionFromMime(const std::string &);
	}
}
