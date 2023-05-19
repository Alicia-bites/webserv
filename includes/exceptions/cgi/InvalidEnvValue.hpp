#pragma once
#include <exception>

namespace webserv {
    namespace exceptions {
        namespace cgi {
            /**
             * @brief Exception thrown when setenv issue
            */
            class InvalidEnvValue: public std::exception {
                public:
                    virtual const char* what() const throw() {
                        return "Env value invalid";
                    }
            };
        }
    }
}
