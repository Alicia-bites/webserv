#pragma once
#include <exception>

namespace webserv {
    namespace exceptions {
        namespace config {
            /**
             * @brief Exception thrown when file is empty
            */
            class InvalidMethods : public std::exception {
                public:
                    virtual const char* what() const throw() {
                        return "Invalid methods vector. Must be of size 3. Set values to \"\" if you want to deactivate it.";
                    }
            };
        }
    }
}
