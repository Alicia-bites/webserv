#pragma once
#include <exception>

namespace webserv {
    namespace exceptions {
        namespace config {
            /**
             * @brief Exception thrown when file name is invalid
            */
            class InvalidFileName : public std::exception {
                public:
                    virtual const char* what() const throw() {
                        return "Invalid file name. Can't open file";
                    }
            };
        }
    }
}
