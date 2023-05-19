#pragma once
#include <exception>

namespace webserv {
    namespace exceptions {
        namespace cgi {
            /**
             * @brief Exception thrown when dup issue
             * 
             */
            class DupFailedChild: public std::exception {
                public:
                    virtual const char* what() const throw() {
                        return "Dup failed on child";
                    }
            };
        }
    }
}
