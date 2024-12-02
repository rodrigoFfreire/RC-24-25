#ifndef CLIENT_TCP_SOCKET_HPP
#define CLIENT_TCP_SOCKET_HPP

#include <string>
#include <memory>
#include <sstream>
#include <netdb.h>
#include <csignal>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>
#include "../exceptions/ClientExceptions.hpp"
#include "../../common/exceptions/SocketErrors.hpp"
#include "../../common/constants.hpp"
#include "../../common/protocol/TCP/tcp.hpp"
#include "../../common/utils.hpp"

class TcpSocket {
private:
    int socket_fd;
    std::string& port;
    std::string& ipaddr;
    std::unique_ptr<struct addrinfo, decltype(&freeaddrinfo)> socket_addr;

    void createSocket();
    void resolveSocket();

public:
    enum Events { OK, TIMEOUT, TERMINATE };

    TcpSocket(std::string& ipaddr, std::string& port)
        : socket_fd(-1), port(port), ipaddr(ipaddr), socket_addr(nullptr, &freeaddrinfo) {};

    ~TcpSocket();

    void setup();
    void receivePacket(std::stringstream& packetStream);
    const addrinfo* getSocketInfo() const;
};

#endif