#pragma once

#include "configFile.hpp"
#include "Response.hpp"
#include <string>
#include <vector>
#include <netinet/in.h>
#include <poll.h>

# define BUFFER_SIZE 4096

namespace webserv {
    namespace io {
        /**
         * @brief Socket class, setups on construction
         * 
         * @note On destrution, closes the connections and all the sockets
         */
        class Socket {
            public:
                Socket(void);
                Socket(std::vector<std::pair<std::string, int> > const & servers);
                ~Socket(void);
                bool good(void);
                void kill(void);
                void acceptConnection(int &socketFd);
                void loop(void (*callback)(webserv::http::Response &response), char** envp, webserv::config::ConfigFile *);
            private:
                std::vector<int> _serverFds;
                char _ipBuffer[INET_ADDRSTRLEN];
                std::vector<std::pair<std::string, int> > _servers;
                std::vector<pollfd> _pfds;
                bool _ready;
                int _openOne();
                bool _open(void);
                void _bindOne(std::pair<std::string, int> &comb, int &fd);
                bool _bind(void);
                void _listenOne(int &fd);
                bool _listen(void);
                void _closeAll(void);
        };
    }
}
