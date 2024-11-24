#include "UdpSocket.hpp"

extern volatile sig_atomic_t terminate_flag;

void UdpSocket::createSocket() {
    if (socket_fd != -1) {
        throw SocketAlreadyCreatedError();
    }

    // Open socket
    if ((socket_fd = socket(socket_addr->ai_family,
                                socket_addr->ai_socktype,
                                socket_addr->ai_protocol)) == -1) {
        throw SocketOpenError();
    }

    // Set read timeout at socket level
    timeval tv;
    tv.tv_sec = SERVER_RECV_TIMEOUT;
    tv.tv_usec = 0;
    if (setsockopt(socket_fd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv)) == -1) {
        throw SocketSetOptFailedError();
    }

    // Bind socket
    if (bind(socket_fd, socket_addr->ai_addr, socket_addr->ai_addrlen) == -1) {
        throw SocketBindError();
    }
}

UdpSocket::~UdpSocket() {
    if (socket_fd != -1) {
        close(socket_fd);
        socket_fd = -1;
    }
}

void UdpSocket::resolveSocket() {
    int gai_err;
    struct addrinfo hints;
    struct addrinfo* raw_addrinfo = nullptr;

    // Create address hints
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_flags = AI_PASSIVE;
    
    // Obtain usable address
    if ((gai_err = getaddrinfo(NULL, this->port.c_str(), &hints, &raw_addrinfo)) != 0) {
        throw SocketAddressResolveError(gai_err);
    }
    socket_addr.reset(raw_addrinfo);
}

void UdpSocket::setup() {
    resolveSocket();
    createSocket();
}

int UdpSocket::receivePacket(std::stringstream& packetStream, struct sockaddr_in& client_addr) {
    char buffer[SOCK_BUFFER_SIZE];
    ssize_t received_bytes;
    socklen_t client_addrlen = sizeof(client_addr);

    received_bytes = recvfrom(socket_fd,
                                        buffer,
                                        sizeof(buffer),
                                        0,
                                        reinterpret_cast<struct sockaddr*>(&client_addr),
                                        &client_addrlen
                                    );
    if (received_bytes == -1) {
        if (errno == EAGAIN || errno == EWOULDBLOCK) {
            return TIMEOUT;
        }
        else if (terminate_flag) {
            return TERMINATE;
        }
        throw ServerReceiveError();
    }

    packetStream.write(buffer, received_bytes);
    return OK;
}

const addrinfo *UdpSocket::getSocketInfo() const {
    return socket_addr.get();
}
