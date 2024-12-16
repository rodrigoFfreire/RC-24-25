#include <atomic>

#include "Server.hpp"

#include "commands/udp_commands.hpp"
#include "commands/tcp_commands.hpp"
#include "../common/constants.hpp"
#include "../common/utils.hpp"


extern std::atomic<bool> terminateFlag;

Server::Server(Config& config, Logger& logger) 
    : _port(config.port), _udpSocket(_port), _tcpSocket(_port), logger(logger), store(config.dataPath) {
    registerCommands();
};

void Server::setupUdp() {
    char ipstr[INET_ADDRSTRLEN];
    std::ostringstream log_msg;
    
    _udpSocket.setup();

    const addrinfo* info = _udpSocket.getSocketInfo();
    struct sockaddr_in *udp_addr = reinterpret_cast<sockaddr_in*>(info->ai_addr);
    inet_ntop(info->ai_family, &udp_addr->sin_addr, ipstr, sizeof(ipstr));
    log_msg << "UDP socket binded to " << ipstr << ":" << _port;

    logger.log(Logger::Severity::INFO, log_msg.str(), true);

}

void Server::setupTcp() {
    char ipstr[INET_ADDRSTRLEN];
    std::ostringstream log_msg;
    
    _tcpSocket.setup();
    _tcpPool.dispatch(TCP_MAXCLIENTS);

    const addrinfo* info = _tcpSocket.getSocketInfo();
    struct sockaddr_in *tcp_addr = reinterpret_cast<sockaddr_in*>(info->ai_addr);
    inet_ntop(info->ai_family, &tcp_addr->sin_addr, ipstr, sizeof(ipstr));
    log_msg << "TCP socket binded and listening at " << ipstr << ":" << _port;
    
    logger.log(Logger::Severity::INFO, log_msg.str(), true);

}

void Server::registerCommands() {
    // Udp commands
    _udp_handlers.insert({StartNewGamePacket::packetID, startNewGameHandler});
    _udp_handlers.insert({TryPacket::packetID, tryHandler});
    _udp_handlers.insert({QuitPacket::packetID, quitHandler});
    _udp_handlers.insert({DebugPacket::packetID, debugGameHandler});

    // Tcp commands
    _tcp_handlers.insert({ShowTrialsPacket::packetID, showTrialsHandler});
    _tcp_handlers.insert({ShowScoreboardPacket::packetID, showScoreboardHandler});
}

void Server::handleUdpCommand(const std::string& packetId, std::stringstream& packetStream, std::unique_ptr<UdpPacket>& replyPacket) {
    auto it = _udp_handlers.find(packetId);
    if (it == _udp_handlers.end()) {
        throw UnexpectedPacketException();
    }

    it->second(packetStream, store, logger, replyPacket);
}

void Server::handleTcpCommand(const std::string &packetId, const int conn_fd, std::unique_ptr<TcpPacket> &replyPacket) {
    auto it = _tcp_handlers.find(packetId);
    if (it == _tcp_handlers.end()) {
        throw UnexpectedPacketException();
    }

    it->second(conn_fd, store, logger, replyPacket);
}

void Server::runUdp() {
    while (!terminateFlag.load()) {
        struct sockaddr_in client_addr;
        std::string response;
        char client_addrstr[INET_ADDRSTRLEN];

        try {
            std::unique_ptr<UdpPacket> replyPacket = nullptr;
            std::stringstream packetStream;
            packetStream >> std::noskipws;

            // Receive packet from client
            int rec = _udpSocket.receivePacket(packetStream, client_addr);
            if (rec == UdpSocket::TIMEOUT)
                continue;
            else if (rec == UdpSocket::TERMINATE)
                break;

            // Get client address and port
            const addrinfo* info = _udpSocket.getSocketInfo();
            inet_ntop(info->ai_family, &client_addr.sin_addr, client_addrstr, sizeof(client_addrstr));

            // Log request (verbose)
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
                response = _udpSocket.sendPacket(replyPacket, client_addr);
        } catch (const CommonException& e) {
            logger.log(Logger::Severity::WARN, e.what(), true);
            try {
                std::unique_ptr<UdpPacket> errPacket = std::make_unique<UdpErrorPacket>();
                response = _udpSocket.sendPacket(errPacket, client_addr);
            } catch (const ServerSendError& e) {
                logger.log(Logger::Severity::ERROR, e.what(), true);
            }
        } catch (const CommonError& e) {
            logger.log(Logger::Severity::WARN, e.what(), true);
            try {
                std::unique_ptr<UdpPacket> errPacket = std::make_unique<UdpErrorPacket>();
                response = _udpSocket.sendPacket(errPacket, client_addr);
            } catch (const ServerSendError& e) {
                logger.log(Logger::Severity::ERROR, e.what(), true);
            }
        } catch (const std::exception& e) {
            logger.log(Logger::Severity::ERROR, e.what(), true);
        }

        if (!response.empty()) {
            std::ostringstream log_msg;
            log_msg << "(UDP) " << '\"' << response << '\"';
            log_msg << " > [" << client_addrstr << ":" << ntohs(client_addr.sin_port) << ']';
            logger.logVerbose(Logger::Severity::INFO, log_msg.str(), true);
        }
    }
    logger.log(Logger::Severity::INFO, "UDP monitor terminated!", true);
}

void Server::runTcp() {
    while (!terminateFlag.load()) {
        struct sockaddr_in client_addr;
        int conn_fd = -1;

        try {
            int err = _tcpSocket.acceptConnection(conn_fd, client_addr);
            if (err == TcpSocket::TIMEOUT)
                continue;
            else if (err == TcpSocket::TERMINATE)
                break;

            // Get client address and port
            const addrinfo* info = _tcpSocket.getSocketInfo();
            char client_addrstr[INET_ADDRSTRLEN];
            inet_ntop(info->ai_family, &client_addr.sin_addr, client_addrstr, sizeof(client_addrstr));

            // Log connection request
            std::ostringstream log_msg;
            log_msg << "(TCP) " << "[" << client_addrstr << ":" << ntohs(client_addr.sin_port) << "] > " << "Connected";
            logger.logVerbose(Logger::Severity::INFO, log_msg.str(), true);

            // Handle connection with worker thread
            _tcpSocket.setupConnection(conn_fd);
            _tcpPool.enqueueConnection([this, conn_fd, client_addrstr, client_addr] {
                handleTcpConnection(conn_fd, client_addrstr, client_addr);
            });
        } catch (const CommonError& e) {
            logger.log(Logger::Severity::ERROR, e.what(), true);
        }
    }
    logger.log(Logger::Severity::INFO, "TCP monitor terminated! Closing worker threads...", true);
}

void Server::handleTcpConnection(const int conn_fd, const char *client_addrstr, const sockaddr_in& client_addr) {
    std::unique_ptr<TcpPacket> replyPacket = nullptr;
    std::string response;

    try {
        // Get packet ID
        TcpParser parser(conn_fd);
        std::string packetID = parser.parsePacketID();

        // Handle command
        handleTcpCommand(packetID, conn_fd, replyPacket);

        // Send reply
        if (replyPacket != nullptr)
            response = replyPacket->send(conn_fd);
    } catch (const CommonException& e) {
        logger.log(Logger::Severity::WARN, e.what(), true);
        try {
            std::unique_ptr<TcpPacket> errPacket = std::make_unique<TcpErrorPacket>();
            response = errPacket->send(conn_fd);
        } catch (const ServerSendError& e) {
            logger.log(Logger::Severity::ERROR, e.what(), true);
        }
    } catch (const CommonError& e) {
        logger.log(Logger::Severity::WARN, e.what(), true);
        try {
            std::unique_ptr<TcpPacket> errPacket = std::make_unique<TcpErrorPacket>();
            response = errPacket->send(conn_fd);
        } catch (const ServerSendError& e) {
            logger.log(Logger::Severity::ERROR, e.what(), true);
        }
    } catch (const std::exception& e) {
        logger.log(Logger::Severity::ERROR, e.what(), true);
    }

    if (!response.empty()) {
        std::ostringstream log_msg;
        log_msg << "(TCP) " << response << " > [ " << client_addrstr << ":" << ntohs(client_addr.sin_port) << "]";
        logger.logVerbose(Logger::Severity::INFO, log_msg.str(), true);
    }

    close(conn_fd);
    return;
}

std::time_t Server::getCommandTime() {
    return std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
}
