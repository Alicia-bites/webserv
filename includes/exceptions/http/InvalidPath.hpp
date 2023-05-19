#pragma once
#include <exception>

namespace webserv {
    namespace exceptions {
        namespace http {
            /**
             * @brief Exception thrown when a path does not exist
            */
            class InvalidPath : public std::exception {
                public:
                    virtual const char* what() const throw() {
                        return "The path you are looking for does not lead anywhere. You might be lost, my friend.";
                    }
            };
        }
    }
}
