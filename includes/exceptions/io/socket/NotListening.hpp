#pragma once
#include <exception>

namespace webserv {
    namespace exceptions {
        namespace io {
            /**
             * @brief Exception thrown when a socket call fails and errno is set to EINVAL
            */
            class SocketNotListening : public std::exception {
                public:
                    virtual const char* what() const throw() {
                        return "Failed to create socket: Socket is not listening";
                    }
            };
        }
    }
}
