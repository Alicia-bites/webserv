#pragma once
#include <exception>

namespace webserv {
    namespace exceptions {
        namespace io {
            /**
             * @brief Exception thrown when a call to socket() fails and errno is set to EMFILE or ENFILE
            */
            class SocketFileLimitReached : public std::exception {
                public:
                    virtual const char* what() const throw() {
                        return "Failed to create socket: System or process file limit reached";
                    }
            };
        }
    }
}
