#ifndef SERVER_HPP
#define SERVER_HPP

#include <unordered_map>
#include <functional>
#include "utils/Config.hpp"
#include "sockets/UdpSocket.hpp"
#include "sockets/TcpSocket.hpp"
#include "../common/Logger.hpp"
#include "../common/protocol/Packet.hpp"
#include "utils/WorkerPool.hpp"

class Server;

class Server {
    typedef void (*HandlerFunc)(std::stringstream&, Server&, std::unique_ptr<Packet>&);

private:
    std::string port;    
    UdpSocket udpSocket;
    TcpSocket tcpSocket;
    std::unordered_map<std::string, HandlerFunc> udp_handlers;
    //std::unordered_map<std::string, HandlerFunc> tcp_handlers;
    WorkerPool tcpPool;

    void setupSockets();
    void registerCommands();
    void handleUdpCommand(std::string& packetId, std::stringstream& packetStream, std::unique_ptr<Packet>& replyPacket);

public:
    Logger& logger;

    Server(Config& config, Logger& logger);
    void runUdp();
    void runTcp();
    void handleTcpConnection(const int conn_fd); // Worker function
};

#endif