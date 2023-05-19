#include "encoding.hpp"
#include "exceptions.hpp"

/**
 * @brief Get the hexadecimal representation of two characters
 * 
 * @param c1 The first character
 * @param c2 The second character
 * 
 * @return char The hexadecimal representation of the two characters (0-255)
 */
char hexParse(char c1, char c2) {
	char value = 0;

	// If the character is a digit, subtract '0' to get the value
	// Otherwise, subtract 'a' and add 10 to get the value
	value += (c1 >= '0' && c1 <= '9') ? c1 - '0' : c1 - 'a' + 10;
	value *= 16;
	value += (c2 >= '0' && c2 <= '9') ? c2 - '0' : c2 - 'a' + 10;

	return value;
}

/**
 * @brief Get the hexadecimal representation of two characters after a %
 * 
 * @param uri Reference to the URI to extract both bytes
 * @param offset Current offset to the %
 * @return char The character represented by the two hexadecimal bytes
 */
char getHexBytes(std::string const &uri, size_t offset) {
	// Check that there's at least 2 characters after the %
	if (offset + 2 > uri.size())
		throw webserv::exceptions::encoding::InvalidEncodedUri();
	
	// Get the next two characters
	std::string hex = uri.substr(offset + 1, 2);

	// Lowercase them to avoid any issues later
	hex[0] = std::tolower(hex[0]);
	hex[1] = std::tolower(hex[1]);

	// Check that both characters are hexadecimal
	if (!std::isxdigit(hex[0]) || !std::isxdigit(hex[1]))
		throw webserv::exceptions::encoding::InvalidEncodedUri();

	// Get their base 10 value
	return hexParse(hex[0], hex[1]);
}

namespace webserv {
	namespace encoding {
		/**
		 * @brief Decode an URI encoded string (supports multi-byte characters)
		 * 
		 * @param uri An encoded URI
		 * @return std::string The decoded URI
		 * 
		 * @throw webserv::exceptions::InvalidEncodedUri If the URI is invalid
		 */
		std::string decodeURI(std::string const &uri)
		{
			std::string output;
			for (size_t i = 0; i < uri.size(); i++) {
				// If the character is not a %, no need to decode anything
				if (uri[i] != '%')
					output += uri[i];
				else {
					output += getHexBytes(uri, i);
					i += 2;
				}
			}
			return output;
		}
	}
}
