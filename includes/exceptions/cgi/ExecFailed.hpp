#pragma once
#include <exception>

namespace webserv {
    namespace exceptions {
        namespace cgi {
            /**
             * @brief Exception thrown when execve issue
             */
            class ExecFailed: public std::exception {
                public:
                    virtual const char* what() const throw() {
                        return "Execve failed";
                    }
            };
        }
    }
}
