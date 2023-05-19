#pragma once
#include <exception>

namespace webserv {
    namespace exceptions {
        namespace http {
            /**
             * @brief Exception thrown when a header is not in the `Key: Value\n` format
            */
            class InvalidCfgRoot : public std::exception {
                public:
                    virtual const char* what() const throw() {
                        return "Config root is not found or is not a directory";
                    }
            };
        }
    }
}
