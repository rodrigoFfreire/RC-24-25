#include "Server.hpp"

#include "../common/constants.hpp"
#include "../common/utils.hpp"
#include "commands/udp_commands.hpp"
#include "commands/tcp_commands.hpp"

extern volatile std::sig_atomic_t terminate_flag;

Server::Server(Config& config, Logger& logger) 
    : port(config.port), udpSocket(port), tcpSocket(port), logger(logger), store(config.dataPath) {
    registerCommands();
};

void Server::setupUdp() {
    char ipstr[INET_ADDRSTRLEN];
    std::ostringstream log_msg;
    
    udpSocket.setup();

    const addrinfo* info = udpSocket.getSocketInfo();
    struct sockaddr_in *udp_addr = reinterpret_cast<sockaddr_in*>(info->ai_addr);
    inet_ntop(info->ai_family, &udp_addr->sin_addr, ipstr, sizeof(ipstr));
    log_msg << "Udp socket binded to " << ipstr << ":" << port;

    logger.log(Logger::Severity::INFO, log_msg.str(), true);

}

void Server::setupTcp() {
    char ipstr[INET_ADDRSTRLEN];
    std::ostringstream log_msg;
    
    tcpSocket.setup();
    tcpPool.dispatch(TCP_MAXCLIENTS);

    const addrinfo* info = tcpSocket.getSocketInfo();
    struct sockaddr_in *tcp_addr = reinterpret_cast<sockaddr_in*>(info->ai_addr);
    inet_ntop(info->ai_family, &tcp_addr->sin_addr, ipstr, sizeof(ipstr));
    log_msg << "TCP socket binded and listening at " << ipstr << ":" << port;
    
    logger.log(Logger::Severity::INFO, log_msg.str(), true);

}

void Server::registerCommands() {
    // Udp commands
    udp_handlers.insert({StartNewGamePacket::packetID, startNewGameHandler});
    udp_handlers.insert({TryPacket::packetID, tryHandler});
    udp_handlers.insert({QuitPacket::packetID, quitHandler});
    udp_handlers.insert({DebugPacket::packetID, debugGameHandler});

    // Tcp commands
    tcp_handlers.insert({ShowTrialsPacket::packetID, showTrialsHandler});
    tcp_handlers.insert({ShowScoreboardPacket::packetID, showScoreboardHandler});
}

void Server::handleUdpCommand(std::string& packetId, std::stringstream& packetStream, std::unique_ptr<UdpPacket>& replyPacket) {
    auto it = udp_handlers.find(packetId);
    if (it == udp_handlers.end()) {
        throw UnexpectedPacketException();
    }

    it->second(packetStream, *this, replyPacket);
}

void Server::handleTcpCommand(std::string &packetId, const int& conn_fd, std::unique_ptr<TcpPacket> &replyPacket) {
    auto it = tcp_handlers.find(packetId);
    if (it == tcp_handlers.end()) {
        throw UnexpectedPacketException();
    }

    it->second(conn_fd, *this, replyPacket);
}

void Server::runUdp() {
    setupUdp();
    while (!terminate_flag) {
        struct sockaddr_in client_addr;
        try {
            std::unique_ptr<UdpPacket> replyPacket = nullptr;
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
            log_msg << "(UDP) " << "[" << client_addrstr << ":" << ntohs(client_addr.sin_port) << "] > ";
            log_msg << '\"' << packetStr << '\"';
            logger.logVerbose(Logger::Severity::INFO, log_msg.str(), true);

            // Get packet ID
            UdpParser parser(packetStream);
            std::string packetID = parser.parsePacketID();

            // Dispatch command
            handleUdpCommand(packetID, packetStream, replyPacket);

            // Send reply
            if (replyPacket != nullptr)
                udpSocket.sendPacket(replyPacket, client_addr);
        } catch (const CommonException& e) {
            logger.log(Logger::Severity::WARN, e.what(), true);
            try {
                std::unique_ptr<UdpPacket> errPacket = std::make_unique<UdpErrorPacket>();
                udpSocket.sendPacket(errPacket, client_addr);
            } catch (const ServerSendError& e) {
                logger.log(Logger::Severity::ERROR, e.what(), true);
            }
        } catch (const CommonError& e) {
            logger.log(Logger::Severity::WARN, e.what(), true);
            try {
                std::unique_ptr<UdpPacket> errPacket = std::make_unique<UdpErrorPacket>();
                udpSocket.sendPacket(errPacket, client_addr);
            } catch (const ServerSendError& e) {
                logger.log(Logger::Severity::ERROR, e.what(), true);
            }
        } catch (const std::exception& e) {
            logger.log(Logger::Severity::ERROR, e.what(), true);
        }
    }
    logger.log(Logger::Severity::INFO, "UDP monitor terminated!", true);
}

void Server::runTcp() {
    setupTcp();
    while (!terminate_flag) {
        struct sockaddr_in client_addr;
        int conn_fd = -1;

        try {
            int err = tcpSocket.acceptConnection(conn_fd, client_addr);
            if (err == TcpSocket::TIMEOUT)
                continue;
            else if (err == TcpSocket::TERMINATE)
                break;

            // Get client address and port
            const addrinfo* info = tcpSocket.getSocketInfo();
            char client_addrstr[INET_ADDRSTRLEN];
            inet_ntop(info->ai_family, &client_addr.sin_addr, client_addrstr, sizeof(client_addrstr));

            // Log connection request
            std::ostringstream log_msg;
            log_msg << "(TCP) " << "[" << client_addrstr << ":" << ntohs(client_addr.sin_port) << "] > " << "Connected";
            logger.logVerbose(Logger::Severity::INFO, log_msg.str(), true);

            // Handle connection with worker thread
            tcpSocket.setupConnection(conn_fd);
            tcpPool.enqueueConnection([this, conn_fd] {
                handleTcpConnection(conn_fd);
            });
        } catch (const CommonError& e) {
            logger.log(Logger::Severity::ERROR, e.what(), true);
        }
    }
    logger.log(Logger::Severity::INFO, "TCP monitor terminated! Closing worker threads...", true);
}

void Server::handleTcpConnection(int conn_fd) {
    std::unique_ptr<TcpPacket> replyPacket = nullptr;

    try {
        // Get packet ID
        TcpParser parser(conn_fd);
        std::string packetID = parser.parsePacketID();

        // Handle command
        handleTcpCommand(packetID, conn_fd, replyPacket);

        // Send reply
        if (replyPacket != nullptr)
            replyPacket->send(conn_fd);
    } catch (const CommonException& e) {
        logger.log(Logger::Severity::WARN, e.what(), true);
        try {
            std::unique_ptr<TcpPacket> errPacket = std::make_unique<TcpErrorPacket>();
            errPacket->send(conn_fd);
        } catch (const ServerSendError& e) {
            logger.log(Logger::Severity::ERROR, e.what(), true);
        }
    } catch (const CommonError& e) {
        logger.log(Logger::Severity::WARN, e.what(), true);
        try {
            std::unique_ptr<TcpPacket> errPacket = std::make_unique<TcpErrorPacket>();
            errPacket->send(conn_fd);
        } catch (const ServerSendError& e) {
            logger.log(Logger::Severity::ERROR, e.what(), true);
        }
    } catch (const std::exception& e) {
        logger.log(Logger::Severity::ERROR, e.what(), true);
    }

    close(conn_fd);
    return;
}

std::time_t Server::getCommandTime() {
    return std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
}
