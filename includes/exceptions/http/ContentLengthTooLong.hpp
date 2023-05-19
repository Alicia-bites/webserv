#pragma once
#include <exception>

namespace webserv {
    namespace exceptions {
        namespace http {
            /**
             * @brief Exception thrown when ContentLengthTooLong
            */
            class ContentLengthTooLong : public std::exception {
                public:
                    virtual const char* what() const throw() {
                        return "Content length is too long.";
                    }
            };
        }
    }
}
