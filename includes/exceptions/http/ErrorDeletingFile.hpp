#pragma once
#include <exception>

namespace webserv {
    namespace exceptions {
        namespace http {
            /**
             * @brief Exception thrown when ErrorDeletingFile
            */
            class ErrorDeletingFile : public std::exception {
                public:
                    virtual const char* what() const throw() {
                        return "There was an error deleting a file.";
                    }
            };
        }
    }
}
