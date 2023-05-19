#pragma once
#include <exception>

namespace webserv {
    namespace exceptions {
        namespace io {
            /**
             * @brief Exception thrown when a call to socket() fails and errno is set to ENOBUFS or ENOMEM
            */
            class SocketOutOfMemory : public std::exception {
                public:
                    virtual const char* what() const throw() {
                        return "Failed to create socket: Out of memory";
                    }
            };
        }
    }
}
