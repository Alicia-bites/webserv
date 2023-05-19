#pragma once
#include <exception>

namespace webserv {
    namespace exceptions {
        namespace http {
            /**
             * @brief Exception thrown when a redirection happens
            */
            class Redirecting : public std::exception {
                public:
                    virtual const char* what() const throw() {
                        return "Redirecting";
                    }
            };
        }
    }
}
