#ifndef SERVER_HPP
#define SERVER_HPP

#include <unordered_map>
#include "utils/Config.hpp"
#include "utils/WorkerPool.hpp"
#include "sockets/UdpSocket.hpp"
#include "sockets/TcpSocket.hpp"
#include "../common/Logger.hpp"

class Server;

class Server {
    typedef void (*HandlerUdpFunc)(std::stringstream&, Server&, std::unique_ptr<UdpPacket>&);
    typedef void (*HandlerTcpFunc)(const int&, Server&, std::unique_ptr<TcpPacket>&);

private:
    std::string port;    
    UdpSocket udpSocket;
    TcpSocket tcpSocket;
    std::unordered_map<std::string, HandlerUdpFunc> udp_handlers;
    std::unordered_map<std::string, HandlerTcpFunc> tcp_handlers;
    WorkerPool tcpPool;

    void setupUdp();
    void setupTcp();
    void registerCommands();
    void handleUdpCommand(std::string& packetId, std::stringstream& packetStream, std::unique_ptr<UdpPacket>& replyPacket);
    void handleTcpCommand(std::string& packetId, const int& conn_fd, std::unique_ptr<TcpPacket>& replyPacket);

public:
    Logger& logger;

    Server(Config& config, Logger& logger);
    void runUdp();
    void runTcp();
    void handleTcpConnection(const int conn_fd); // Worker function
};

#endif