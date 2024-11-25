#include "Server.hpp"

#include "../common/constants.hpp"
#include "../common/protocol/udp.hpp"
#include "commands/udp_commands.hpp"

extern volatile std::sig_atomic_t terminate_flag;

Server::Server(Config& config, Logger& logger) 
    : port(config.port), udpSocket(port), tcpSocket(port), tcpPool(TCP_MAXCLIENTS), logger(logger) {
    registerCommands();
    setupSockets();
};

void Server::setupSockets() {
    char ipstr[INET_ADDRSTRLEN];
    std::ostringstream log_msg;
    
    udpSocket.setup();
    tcpSocket.setup();

    const addrinfo* info = udpSocket.getSocketInfo();
    struct sockaddr_in *udp_addr = reinterpret_cast<sockaddr_in*>(info->ai_addr);
    inet_ntop(info->ai_family, &udp_addr->sin_addr, ipstr, sizeof(ipstr));
    log_msg << "Udp socket binded to " << ipstr << ":" << this->port << " | ";

    info = tcpSocket.getSocketInfo();
    struct sockaddr_in *tcp_addr = reinterpret_cast<sockaddr_in*>(info->ai_addr);
    inet_ntop(info->ai_family, &tcp_addr->sin_addr, ipstr, sizeof(ipstr));
    log_msg << "Tcp socket binded and listening at " << ipstr << ":" << port << std::endl;

    logger.log(Logger::Severity::INFO, log_msg.str());
}

void Server::registerCommands() {
    // Udp commands
    udp_handlers.insert({StartNewGamePacket::packetID, startNewGameHandler});
    udp_handlers.insert({TryPacket::packetID, tryHandler});
    udp_handlers.insert({QuitPacket::packetID, quitHandler});
    udp_handlers.insert({DebugPacket::packetID, debugGameHandler});

    // Tcp commands
}

void Server::handleUdpCommand(std::string& packetId, std::stringstream& packetStream, std::unique_ptr<Packet>& replyPacket) {
    auto it = udp_handlers.find(packetId);
    if (it == udp_handlers.end()) {
        throw UnexpectedPacketException();
    }

    it->second(packetStream, *this, replyPacket);
}

void Server::runUdp() {
    while (!terminate_flag) {
        struct sockaddr_in client_addr;
        try {
            std::unique_ptr<Packet> replyPacket = nullptr;
            std::stringstream packetStream;
            packetStream >> std::noskipws;

            // Receive packet from client
            int rec = udpSocket.receivePacket(packetStream, client_addr);
            if (rec == UdpSocket::TIMEOUT)
                continue;
            else if (rec == UdpSocket::TERMINATE)
                break;

            // Get client address and port
            const addrinfo* info = udpSocket.getSocketInfo();
            char client_addrstr[INET_ADDRSTRLEN];
            inet_ntop(info->ai_family, &client_addr.sin_addr, client_addrstr, sizeof(client_addrstr));

            // Log client request
            std::ostringstream log_msg;
            std::string packetStr = packetStream.str();
            log_msg << "[" << client_addrstr << ":" << ntohs(client_addr.sin_port) << "] > " << '\"' << packetStr << '\"' << std::endl;
            logger.logVerbose(Logger::Severity::INFO, log_msg.str(), (packetStr.back() == '\n' ? false : true));

            // Get packet ID
            PacketParser parser(packetStream);
            std::string packetID = parser.parsePacketID();

            // Dispatch command
            handleUdpCommand(packetID, packetStream, replyPacket);

            // Send reply
            if (replyPacket != nullptr)
                udpSocket.sendPacket(replyPacket, client_addr);
        } catch (const CommonException& e) {
            logger.log(Logger::Severity::WARN, e.what(), true);
            try {
                std::unique_ptr<Packet> errPacket = std::make_unique<ErrorPacket>();
                udpSocket.sendPacket(errPacket, client_addr);
            } catch (const ServerSendError& e) {
                logger.log(Logger::Severity::ERROR, e.what(), true);
            }
        } catch (const CommonError& e) {
            logger.log(Logger::Severity::WARN, e.what(), true);
            try {
                std::unique_ptr<Packet> errPacket = std::make_unique<ErrorPacket>();
                udpSocket.sendPacket(errPacket, client_addr);
            } catch (const ServerSendError& e) {
                logger.log(Logger::Severity::ERROR, e.what(), true);
            }
        } catch (const std::exception& e) {
            logger.log(Logger::Severity::ERROR, e.what(), true);
        }
    }
}

void Server::runTcp() {
    while (!terminate_flag) {
        struct sockaddr_in client_addr;
        int conn_fd = -1;
        try {
            int err = tcpSocket.acceptConnection(conn_fd, client_addr);
            if (err == TcpSocket::TERMINATE)
                continue;
            else if (err == TcpSocket::TERMINATE)
                break;

            std::string a("Incoming request! fd=", conn_fd);
            logger.log(Logger::Severity::INFO, a, true);
            tcpPool.enqueueConnection([this, conn_fd] {
                handleTcpConnection(conn_fd);
            });
        } catch (...) {
            // handle errors
        }
    }
}

void Server::handleTcpConnection(const int conn_fd) {
    usleep(3000000);
    std::string m("TCP connection established on fd=" + conn_fd + '\n');
    write(conn_fd, m.c_str(), m.size());
    logger.log(Logger::Severity::INFO, m, true);
    close(conn_fd);
}