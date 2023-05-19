#pragma once
#include <exception>

namespace webserv {
    namespace exceptions {
        namespace cgi {
            /**
             * @brief Exception thrown when fork issue
             */
            class ForkFailed: public std::exception {
                public:
                    virtual const char* what() const throw() {
                        return "Fork failed";
                    }
            };
        }
    }
}
