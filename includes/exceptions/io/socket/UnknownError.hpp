#pragma once
#include <exception>

namespace webserv {
    namespace exceptions {
        namespace io {
            /**
             * @brief Exception thrown when a call to socket() fails and errno is set to EPROTONOSUPPORT
            */
            class SocketUnknownError : public std::exception {
                public:
                    virtual const char* what() const throw() {
                        return "Failed to create socket: An unknown error occured";
                    }
            };
        }
    }
}
