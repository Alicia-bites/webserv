#pragma once
#include <exception>

namespace webserv {
    namespace exceptions {
        namespace http {
            /**
             * @brief Exception thrown when the request line is badly formatted
            */
            class InvalidRequestLine : public std::exception {
                public:
                    virtual const char* what() const throw() {
                        return "Request line is badly formatted";
                    }
            };
        }
    }  
}
