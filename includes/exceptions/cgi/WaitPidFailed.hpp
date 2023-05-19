#pragma once
#include <exception>

namespace webserv {
    namespace exceptions {
        namespace cgi {
            /**
             * @brief Exception thorwn when waitpid issue
             */
            class WaitPidFailed: public std::exception {
                public:
                    virtual const char* what() const throw() {
                        return "WaitPid failed";
                    }
            };
        }
    }
}
