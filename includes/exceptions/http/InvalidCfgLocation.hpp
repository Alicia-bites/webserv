#pragma once
#include <exception>

namespace webserv {
    namespace exceptions {
        namespace http {
            /**
             * @brief Exception thrown when a header is not in the `Key: Value\n` format
            */
            class InvalidCfgLocation : public std::exception {
                public:
                    virtual const char* what() const throw() {
                        return "The location you are looking for has not been found in configuration file, or does not have required method allowed.";
                    }
            };
        }
    }
}
