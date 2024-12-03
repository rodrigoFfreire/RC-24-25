#include "TcpSocket.hpp"

extern volatile std::sig_atomic_t terminate_flag;

void TcpSocket::createSocket() {
    if (socket_fd != -1) {
        throw SocketAlreadyCreatedError();
    }

    // Open socket
    if ((socket_fd = socket(socket_addr->ai_family,
                                socket_addr->ai_socktype,
                                socket_addr->ai_protocol)) == -1) {
        throw SocketOpenError();
    }

    // Set reusable address and port to avoid "already in use" errors and allow multiple tcp threads
    int yes = 1;
    if (setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) {
        throw SocketSetOptError();
    }

    // Set read/write timeout at socket level
    struct timeval tv;
    tv.tv_sec = SERVER_RECV_TIMEOUT;
    tv.tv_usec = 0;
    if (setsockopt(socket_fd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv)) == -1) {
        throw SocketSetOptError();
    }

    tv.tv_sec = CLIENT_SEND_TIMEOUT;
    tv.tv_usec = 0;
    if (setsockopt(socket_fd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv)) == -1) {
        throw SocketSetOptError();
    }
}

TcpSocket::~TcpSocket() {
    if (socket_fd != -1) {
        close(socket_fd);
        socket_fd = -1;
    }
}

void TcpSocket::resolveSocket() {
    int gai_err;
    struct addrinfo hints;
    struct addrinfo* raw_addrinfo = nullptr;

    // Create address hints
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    
    // Obtain usable address
    if ((gai_err = getaddrinfo(ipaddr.c_str(), this->port.c_str(), &hints, &raw_addrinfo)) != 0) {
        throw SocketAddressResolveError(gai_err);
    }
    socket_addr.reset(raw_addrinfo);
}

void TcpSocket::setup() {
    resolveSocket();
    createSocket();
    if (connect(socket_fd, socket_addr->ai_addr, socket_addr->ai_addrlen) == -1) {
        throw ConnectTCPError();
    }
}

void TcpSocket::receivePacket(TcpPacket *packet) {
    if (packet == nullptr)
        return;

    packet->read(socket_fd);
}

void TcpSocket::sendPacket(TcpPacket *packet) {
    if (packet == nullptr)
        return;

    packet->send(socket_fd);
}

const addrinfo *TcpSocket::getSocketInfo() const {
    return socket_addr.get();
}
