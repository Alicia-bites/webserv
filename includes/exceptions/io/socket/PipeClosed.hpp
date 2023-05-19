#pragma once
#include <exception>

namespace webserv {
    namespace exceptions {
        namespace io {
            /**
             * @brief Exception thrown when a write/read call on a socket fails because the socket is closed
            */
            class PipeClosed : public std::exception {
                public:
                    virtual const char* what() const throw() {
                        return "socket I/O: Pipe closed";
                    }
            };
        }
    }
}
