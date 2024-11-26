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

    // Set listening timeout at socket level
    struct timeval tv;
    tv.tv_sec = SERVER_RECV_TIMEOUT;
    tv.tv_usec = 0;
    if (setsockopt(socket_fd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv)) == -1) {
        throw SocketSetOptError();
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

void TcpSocket::setupConnection(const int &conn_fd) {
    // Set receive and send timeouts for each connection
    struct timeval tv;
    tv.tv_sec = TCP_CONN_RECV_TIMEOUT;
    tv.tv_usec = 0;
    if (setsockopt(conn_fd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv)) == -1) {
        throw SocketSetOptError();
    }

    tv.tv_sec = TCP_CONN_SEND_TIMEOUT;
    if (setsockopt(conn_fd, SOL_SOCKET, SO_SNDTIMEO, &tv, sizeof(tv)) == -1) {
        throw SocketSetOptError();
    }
}

void TcpSocket::sendPacket(const int &conn_fd, std::unique_ptr<Packet> &replyPacket) {
    std::string packetStr = replyPacket->encode();
    const char *buffer = packetStr.c_str();

    ssize_t written_bytes = safe_write(conn_fd, buffer, packetStr.size());
    if (written_bytes == -1) {
        if (errno == EAGAIN || errno == EWOULDBLOCK) {
            throw ConnectionTimeoutError();
        }
        throw ServerSendError();
    }
}

const addrinfo *TcpSocket::getSocketInfo() const {
    return socket_addr.get();
}
