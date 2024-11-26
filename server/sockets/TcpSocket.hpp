#ifndef SERVER_TCP_SOCKET_HPP
#define SERVER_TCP_SOCKET_HPP

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
#include "../../common/protocol/Packet.hpp"
#include "../../common/utils.hpp"

class TcpSocket {
private:
    int socket_fd;
    std::string port;
    std::unique_ptr<struct addrinfo, decltype(&freeaddrinfo)> socket_addr;

    void createSocket();
    void resolveSocket();

public:
    enum Events { OK, TIMEOUT, TERMINATE };

    TcpSocket(std::string port)
        : socket_fd(-1), port(port), socket_addr(nullptr, &freeaddrinfo) {};

    ~TcpSocket();

    void setup();
    int acceptConnection(int& conn_fd, struct sockaddr_in& client_addr);
    void setupConnection(const int& conn_fd);
    void sendPacket(const int& conn_fd, std::unique_ptr<Packet>& replyPacket);
    const addrinfo* getSocketInfo() const;
};

#endif