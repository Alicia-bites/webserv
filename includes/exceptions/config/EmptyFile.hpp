#pragma once
#include <exception>

namespace webserv {
    namespace exceptions {
        namespace config {
            /**
             * @brief Exception thrown when file is empty
            */
            class EmptyFile : public std::exception {
                public:
                    virtual const char* what() const throw() {
                        return "Empty file, invalid type or cannot open file. Nothing to parse.";
                    }
            };
        }
    }
}
