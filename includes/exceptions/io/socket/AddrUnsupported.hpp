#pragma once
#include <exception>

namespace webserv {
    namespace exceptions {
        namespace io {
            /**
             * @brief Exception thrown when a call to socket() fails and errno is set to EAFNOSUPPORT
            */
            class SocketAddrUnsupported : public std::exception {
                public:
                    virtual const char* what() const throw() {
                        return "Failed to create socket: Address family not supported by protocol";
                    }
            };
        }
    }
}
