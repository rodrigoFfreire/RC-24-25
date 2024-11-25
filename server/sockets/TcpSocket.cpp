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

    // Bind socket
    if (bind(socket_fd, socket_addr->ai_addr, socket_addr->ai_addrlen) == -1) {
        throw SocketBindError();
    }

    // Start listening
    if (listen(socket_fd, TCP_BACKLOG) == -1) {
        throw SocketListenError();
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
    hints.ai_flags = AI_PASSIVE;
    
    // Obtain usable address
    if ((gai_err = getaddrinfo(NULL, this->port.c_str(), &hints, &raw_addrinfo)) != 0) {
        throw SocketAddressResolveError(gai_err);
    }
    socket_addr.reset(raw_addrinfo);
}

void TcpSocket::setup() {
    resolveSocket();
    createSocket();
}

int TcpSocket::acceptConnection(int& conn_fd, struct sockaddr_in& client_addr) {
    socklen_t client_addrlen = sizeof(client_addr);

    conn_fd = accept(socket_fd,
                        reinterpret_cast<struct sockaddr*>(&client_addr),
                        &client_addrlen);
    if (conn_fd == -1) {
        if (errno == EAGAIN || errno == EWOULDBLOCK) {
            return TIMEOUT;
        } else if (terminate_flag) {
            return TERMINATE;
        }
        throw AcceptTCPConnectionError();
    }
    return OK;
}

const addrinfo *TcpSocket::getSocketInfo() const {
    return socket_addr.get();
}
