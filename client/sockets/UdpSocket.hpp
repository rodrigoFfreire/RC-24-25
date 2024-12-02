#ifndef CLIENT_UDP_SOCKET_HPP
#define CLIENT_UDP_SOCKET_HPP

#include <string>
#include <memory>
#include <sstream>
#include <iostream>
#include <netdb.h>
#include <csignal>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>
#include "../../common/exceptions/SocketErrors.hpp"
#include "../exceptions/ClientExceptions.hpp"
#include "../../common/constants.hpp"
#include "../../common/protocol/UDP/udp.hpp"

class UdpSocket {
private:
    int socket_fd;
    std::string& port;
    std::string& ipaddr;
    std::unique_ptr<struct addrinfo, decltype(&freeaddrinfo)> server_addr;

    void createSocket();
    void resolveSocket();

public:
    enum Events { OK, READ_TIMEOUT, WRITE_TIMEOUT, TERMINATE };

    UdpSocket(std::string& ipaddr, std::string& port)
        : socket_fd(-1), port(port), ipaddr(ipaddr), server_addr(nullptr, &freeaddrinfo) {};

    ~UdpSocket();

    void setup();
    int receivePacket(std::stringstream& packetStream);
    int sendPacket(UdpPacket *packet);

    const struct addrinfo* getSocketInfo() const;
};

#endif