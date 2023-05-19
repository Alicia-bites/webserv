#pragma once
#include <exception>

namespace webserv {
    namespace exceptions {
        namespace http {
            /**
             * @brief Exception thrown when someone is trying to do something forbidden
            */
            class Forbidden : public std::exception {
                public:
                    virtual const char* what() const throw() {
                        return "You are trying to do something forbidden you little thug...";
                    }
            };
        }
    }
}
