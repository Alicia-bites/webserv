#include "Socket.hpp"
#include "Response.hpp"
#include "exceptions.hpp"
#include "logger.hpp"
#include "codes.hpp"
#include <iostream>
#include <errno.h>
#include <unistd.h>
#include <poll.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <fcntl.h>

#define MAX_CONNECTIONS 1024     // Maximum number of connections (for epoll)
#define CONNECTION_TIMEOUT 60000 // Timeout for epoll_wait (in ms) (60 seconds)

using namespace webserv::io;

void *getClientAddress(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET)
        return &(((struct sockaddr_in *)sa)->sin_addr);
    return &(((struct sockaddr_in6 *)sa)->sin6_addr);
}

Socket::Socket(void)
{
    webserv::logger::debug("[socket] Creating a void Socket");
}

Socket::Socket(std::vector<std::pair<std::string, int> > const &servers) : _servers(servers), _ready(false)
{
    webserv::logger::debug("[socket] Creating a Socket");
    for (size_t i = 0; i < _servers.size(); i++)
    {
        if (_servers[i].second < 1 || _servers[i].second > 65535) // 1-65535, 0 is reserved for TCP
            throw webserv::exceptions::io::SocketInvalidPort();
    }
    bzero(this->_ipBuffer, INET_ADDRSTRLEN);
    webserv::logger::debug("[socket] Attempting to open the Socket");
    try {
        if (!_open())
        {
            webserv::logger::error("[socket] Failed to open the Socket");
            switch (errno)
            {
            case EACCES:
                throw webserv::exceptions::io::SocketPermissionDenied();
            case EAFNOSUPPORT:
                throw webserv::exceptions::io::SocketAddrUnsupported();
            case EINVAL:
                throw webserv::exceptions::io::SocketInvalidProtocol();
            case EMFILE:
            case ENFILE:
                throw webserv::exceptions::io::SocketFileLimitReached();
            case ENOBUFS:
            case ENOMEM:
                throw webserv::exceptions::io::SocketOutOfMemory();
            case EPROTONOSUPPORT:
                throw webserv::exceptions::io::SocketProtocolUnsupported();
            default:
                throw webserv::exceptions::io::SocketUnknownError();
            }
        }
        webserv::logger::debug("[socket] Socket opened! Attempting to bind");
        if (!_bind())
        {
            webserv::logger::error("[socket] Unable to bind, closing Socket");
            switch (errno)
            {
            case EACCES:
                throw webserv::exceptions::io::SocketPermissionDenied();
            case EADDRINUSE:
                throw webserv::exceptions::io::SocketAddrInUse();
            case ENOTSOCK:
            case EBADF:
                throw webserv::exceptions::io::SocketInvalidDescriptor();
            case EINVAL:
                throw webserv::exceptions::io::SocketInvalidProtocol();
            case EADDRNOTAVAIL:
                throw webserv::exceptions::io::SocketAddrNotAvailable();
            default:
                throw webserv::exceptions::io::SocketUnknownError();
            }
        }
        webserv::logger::debug("[socket] Socket bound! Attempting to listen");
        if (!_listen())
        {
            webserv::logger::error("[socket] Unable to listen, closing Socket");
            _closeAll();
            switch (errno)
            {
            case EADDRINUSE:
                throw webserv::exceptions::io::SocketAddrInUse();
            case EBADF:
            case ENOTSOCK:
                throw webserv::exceptions::io::SocketInvalidDescriptor();
            case EADDRNOTAVAIL:
                throw webserv::exceptions::io::SocketAddrNotAvailable();
            default:
                throw webserv::exceptions::io::SocketUnknownError();
            }
        }
        webserv::logger::debug("[socket] Socket is ready! Running optimizations");
        try
        {
            _pfds.reserve(SOMAXCONN);
            webserv::logger::debug("[socket] Optimizations complete!");
        }
        catch (std::exception &e)
        {
            webserv::logger::error("[socket] Unable to optimize Socket, out of memory");
            throw webserv::exceptions::io::SocketOutOfMemory();
        }
        // Push all server fds to _pfds
        for (size_t i = 0; i < _serverFds.size(); i++)
        {
            _pfds.push_back(pollfd());
            _pfds[i].fd = _serverFds[i];
            _pfds[i].events = POLLIN;
        }
        webserv::logger::info("[socket] Socket ready !");
        this->_ready = true;
    } catch (std::exception &e) {
        webserv::logger::debug("[socket] Initializing Socket failed, closing Sockets");
        for (size_t i = 0; i < _serverFds.size(); i++)
        {
            close(_serverFds[i]);
        }
        throw;
    }
}

Socket::~Socket(void)
{
    if (_pfds.size() > 0)
    {
        _ready = false;
        webserv::logger::debug("[socket] Closing Socket");
        kill();
    }
}

/**
 * @brief Kill all clients, and the server
 *
 * @throws webserv::exceptions::io::SocketInvalidDescriptor if close() fails
 */
void Socket::kill(void)
{
    webserv::logger::debug("[socket] Shutting down socket");
    for (size_t i = 0; i < _pfds.size(); i++)
    {
        shutdown(_pfds[i].fd, SHUT_RDWR);
    }
    webserv::logger::debug("[socket] Killing all clients");
    _closeAll();
    webserv::logger::info("[socket] Socket killed successfully !");
}

void Socket::acceptConnection(int &socketFd)
{
    struct sockaddr_in client_addr;
    socklen_t csin_len = sizeof(client_addr);
    int clientFd = accept(socketFd, (struct sockaddr *)&client_addr, &csin_len);
    if (clientFd == -1)
        return;
    pollfd clientPfd;
    clientPfd.fd = clientFd;
    clientPfd.events = POLLIN | POLLOUT;
    clientPfd.revents = 0;

    // Add the client to the list of clients
    _pfds.push_back(clientPfd);
}

/**
 * @brief Non-blocking variant of waitForClient(), the callback is called when a client connects.
 *
 * @param callback Handler for the clients
 *
 * @see https://beej.us/guide/bgnet/html/#poll
 */
void Socket::loop(void (*callback)(webserv::http::Response &response), char **envp, webserv::config::ConfigFile *config)
{
    // pfds = array of sockets to monitor and for what, _pfds.size() --> number of sockets to monitor. Poll returns the
    //  number of element(s) in the array that have had a event occur.
    if (poll(&_pfds[0], _pfds.size(), CONNECTION_TIMEOUT) == -1)
        return;
    bool serverHasEvent = false;
    for (size_t i = 0; i < _pfds.size(); i++)
    {
        if (_pfds[i].revents & (POLLIN | POLLOUT))
        {
            acceptConnection(_pfds[i].fd);
            serverHasEvent = true;
        }
    }
    if (!serverHasEvent) // If the server has no event, no need to check the clients
        return;
    for (size_t i = _serverFds.size(); i < _pfds.size(); i++)
    {
        if (_pfds[i].revents & (POLLIN | POLLOUT)) // If a client socket has an event
        {
            try
            {
                struct sockaddr_in serverAddress;
                socklen_t addressLength = sizeof(serverAddress);
                webserv::http::Request request(_pfds[i].fd, config);
                if (getsockname(_pfds[i].fd, (struct sockaddr *)&serverAddress, &addressLength) != -1)
                    request.port = ntohs(serverAddress.sin_port);
                request.setEnv(envp);
                webserv::http::Response response(request, _pfds[i].fd, config);
                callback(response);
            }
            catch (webserv::exceptions::io::PipeClosed &e)
            {
                webserv::logger::error("[socket] Pipe closed");
                shutdown(_pfds[i].fd, SHUT_RDWR);
                close(_pfds[i].fd);
                _pfds.erase(_pfds.begin() + i);
                return;
            }
            shutdown(_pfds[i].fd, SHUT_RDWR);
            close(_pfds[i].fd);
            _pfds.erase(_pfds.begin() + i);
        }
    }
}

int Socket::_openOne(void)
{
    int fd = socket(AF_INET, SOCK_STREAM | O_NONBLOCK, 0);
    int opt = 1;
    if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)))
        throw webserv::exceptions::io::SocketUnknownError();
    if (fd < 0)
        throw webserv::exceptions::io::SocketUnknownError();
    return fd;
}

bool Socket::_open(void)
{
    for (size_t i = 0; i < _servers.size(); i++)
    {
        try
        {
            _serverFds.push_back(_openOne());
        }
        catch (std::exception &e)
        {
            webserv::logger::error("[socket] Failed to open a Socket.");
            return false;
        }
    }
    return true;
}

void Socket::_bindOne(std::pair<std::string, int> &comb, int &fd)
{
    std::string _ip = comb.first;
    int _port = comb.second;
    sockaddr_in server_address;
    server_address.sin_family = AF_INET;    // IPv4
    server_address.sin_port = htons(_port); // htons to convert it to network byte order -- big endian
    if (inet_pton(AF_INET, _ip.c_str(), &server_address.sin_addr) <= 0)
    {
        webserv::logger::error("[socket] Invalid IP address, trying to resolve hostname");
        struct hostent *host = gethostbyname(_ip.c_str());
        if (host == NULL)
        {
            webserv::logger::error("[socket] Unable to resolve hostname");
            throw webserv::exceptions::io::SocketUnknownError();
        }
        server_address.sin_addr = *(struct in_addr *)host->h_addr;
        webserv::logger::info("[socket] Hostname resolved successfully");
    }

    // Bind socket
    if (bind(fd, (sockaddr *)&server_address, sizeof(server_address)))
        throw webserv::exceptions::io::SocketUnknownError();
}

bool Socket::_bind(void)
{
    for (size_t i = 0; i < _servers.size(); i++)
    {
        try
        {
            _bindOne(_servers[i], _serverFds[i]);
        }
        catch (std::exception &e)
        {
            webserv::logger::error("[socket] Failed to bind a Socket.");
            return false;
        }
    }
    return true;
}

void Socket::_listenOne(int &fd)
{
    if (listen(fd, SOMAXCONN) != 0)
        throw webserv::exceptions::io::SocketUnknownError();
}

bool Socket::_listen(void)
{
    for (size_t i = 0; i < _servers.size(); i++)
    {
        try
        {
            _listenOne(_serverFds[i]);
        }
        catch (std::exception &e)
        {
            webserv::logger::error("[socket] Failed to listen on a Socket.");
            return false;
        }
    }
    return true;
}

void Socket::_closeAll(void)
{
    // loop through all pfds, _serverFds and close them
    for (size_t i = 0; i < _pfds.size(); i++)
    {
        shutdown(_pfds[i].fd, SHUT_RDWR);
        close(_pfds[i].fd);
    }
}
