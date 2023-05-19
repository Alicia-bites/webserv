#pragma once
#include <exception>

namespace webserv {
    namespace exceptions {
        namespace io {
            /**
             * @brief Exception thrown when a call to a socket function fails and errno is set to EADDRINUSE
            */
            class SocketAddrInUse : public std::exception {
                public:
                    virtual const char* what() const throw() {
                        return "Socket operation failed: Address already in use";
                    }
            };
        }
    }
}
