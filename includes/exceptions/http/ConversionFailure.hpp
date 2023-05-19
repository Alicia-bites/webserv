#pragma once
#include <exception>

namespace webserv {
    namespace exceptions {
        namespace http {
            /**
             * @brief Exception thrown when ConversionFailure
            */
            class ConversionFailure : public std::exception {
                public:
                    virtual const char* what() const throw() {
                        return "A conversion has failed";
                    }
            };
        }
    }
}
