#pragma once
#include <exception>

namespace webserv {
    namespace exceptions {
        namespace io {
            /**
             * @brief Exception thrown when a call to a socket function fails and errno is set to EBADF
            */
            class SocketInvalidDescriptor : public std::exception {
                public:
                    virtual const char* what() const throw() {
                        return "Socket operation failed: Invalid descriptor";
                    }
            };
        }
    }
}
