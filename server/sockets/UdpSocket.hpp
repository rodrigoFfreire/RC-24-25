#ifndef SERVER_UDP_SOCKET_HPP
#define SERVER_UDP_SOCKET_HPP

#include <string>
#include <memory>
#include <sstream>
#include <netdb.h>
#include <csignal>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>
#include "../exceptions/ServerExceptions.hpp"
#include "../../common/exceptions/SocketErrors.hpp"
#include "../../common/constants.hpp"
#include "../../common/protocol/UDP/udp.hpp"

class UdpSocket {
private:
    int socket_fd;
    std::string port;
    std::unique_ptr<struct addrinfo, decltype(&freeaddrinfo)> socket_addr;

    void createSocket();
    void resolveSocket();

public:
    enum Events { OK, TIMEOUT, TERMINATE };

    UdpSocket(std::string port)
        : socket_fd(-1), port(port), socket_addr(nullptr, &freeaddrinfo) {};

    ~UdpSocket();

    void setup();
    int receivePacket(std::stringstream& packetStream, struct sockaddr_in& client_addr);
    std::string sendPacket(std::unique_ptr<UdpPacket>& replyPacket, struct sockaddr_in& client_addr);

    const struct addrinfo* getSocketInfo() const;
};

#endif