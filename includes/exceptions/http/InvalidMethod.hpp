#pragma once
#include <exception>

namespace webserv {
    namespace exceptions {
        namespace http {
            /**
             * @brief Exception thrown when the request method isn't valid
            */
            class InvalidMethod : public std::exception {
                public:
                    virtual const char* what() const throw() {
                        return "Method isn't valid / supported.";
                    }
            };
        }
    }  
}
