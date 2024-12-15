#ifndef SERVER_HPP
#define SERVER_HPP

#include <unordered_map>
#include <memory>
#include <string>

#include "GameStore.hpp"
#include "utils/Config.hpp"
#include "utils/WorkerPool.hpp"
#include "sockets/UdpSocket.hpp"
#include "sockets/TcpSocket.hpp"
#include "../common/Logger.hpp"

class Server {
    typedef void (*HandlerUdpFunc)(std::stringstream&, GameStore&, Logger&, std::unique_ptr<UdpPacket>&);
    typedef void (*HandlerTcpFunc)(const int, GameStore&, Logger&, std::unique_ptr<TcpPacket>&);

private:
    std::string _port;    
    UdpSocket _udpSocket;
    TcpSocket _tcpSocket;
    std::unordered_map<std::string, HandlerUdpFunc> _udp_handlers;
    std::unordered_map<std::string, HandlerTcpFunc> _tcp_handlers;
    WorkerPool _tcpPool;

    void registerCommands();
    void handleUdpCommand(const std::string& packetId, std::stringstream& packetStream, std::unique_ptr<UdpPacket>& replyPacket);
    void handleTcpCommand(const std::string& packetId, const int conn_fd, std::unique_ptr<TcpPacket>& replyPacket);

public:
    Logger& logger;
    GameStore store;

    Server(Config& config, Logger& logger);
    void setupUdp();
    void setupTcp();
    void runUdp();
    void runTcp();
    void handleTcpConnection(const int conn_fd);
    std::time_t getCommandTime();
};

#endif