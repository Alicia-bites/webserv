#pragma once
#include <exception>

namespace webserv {
    namespace exceptions {
        namespace http {
            /**
             * @brief Exception thrown when ContentOfCurrentPageHasBeenDeleted
            */
            class ContentOfCurrentPageHasBeenDeleted : public std::exception {
                public:
                    virtual const char* what() const throw() {
                        return "The content of the current page has been deleted.";
                    }
            };
        }
    }
}
