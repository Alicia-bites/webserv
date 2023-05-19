#pragma once
#include <exception>

namespace webserv {
    namespace exceptions {
        namespace http {
            /**
             * @brief Exception thrown when a file that should be served cannot be accessed (read)
            */
            class FileAccessDenied : public std::exception {
                public:
                    virtual const char* what() const throw() {
                        return "File access denied";
                    }
            };
        }
    }
}
