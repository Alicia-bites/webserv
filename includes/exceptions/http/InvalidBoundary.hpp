#pragma once
#include <exception>

namespace webserv {
    namespace exceptions {
        namespace http {
            /**
             * @brief Exception thrown when boundary delimiter issue (length, not present)
            */
            class InvalidBoundary: public std::exception {
                public:
                    virtual const char* what() const throw() {
                        return "The boundary delimiter issue: not found, length too large.";
                    }
            };
        }
    }
}
