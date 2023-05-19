#pragma once
#include <exception>

namespace webserv {
    namespace exceptions {
        namespace http {
            /**
             * @brief Exception thrown when the request line is badly formatted
            */
            class UriTooLong : public std::exception {
                public:
                    virtual const char* what() const throw() {
                        return "URI is too long.";
                    }
            };
        }
    }  
}
