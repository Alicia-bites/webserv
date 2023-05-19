#pragma once
#include <exception>

namespace webserv {
    namespace exceptions {
        namespace http {
            /**
             * @brief Exception thrown when upload dir does not exist
            */
            class InvalidUploadDir : public std::exception {
                public:
                    virtual const char* what() const throw() {
                        return "The upload directory does not exist.";
                    }
            };
        }
    }
}
