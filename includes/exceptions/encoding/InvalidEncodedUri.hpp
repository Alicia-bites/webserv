#pragma once
#include <exception>

namespace webserv {
    namespace exceptions {
        namespace encoding {
            /**
             * @brief Exception thrown when the encoded URI is in an invalid format
            */
            class InvalidEncodedUri : public std::exception {
                public:
                    virtual const char* what() const throw() {
                        return "Invalid encoded URI";
                    }
            };
        }
    }
}
