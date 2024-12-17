#include "TcpSocket.hpp"

#include <atomic>

extern std::atomic<bool> terminateFlag;

/// @brief Creates, binds and starts listening on the TCP socket
void TcpSocket::createSocket() {
  if (socket_fd != -1) {
    throw SocketAlreadyCreatedError();
  }

  // Open socket
  if ((socket_fd = socket(socket_addr->ai_family, socket_addr->ai_socktype,
                          socket_addr->ai_protocol)) == -1) {
    throw SocketOpenError();
  }

  // Set reusable address and port to avoid "already in use" errors and allow multiple tcp
  // threads
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

/// @brief Closes the TCP socket
TcpSocket::~TcpSocket() {
  if (socket_fd != -1) {
    close(socket_fd);
    socket_fd = -1;
  }
}

/// @brief Resolves an usable address for the socket
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

/// @brief Sets up the socket (open, resolve, bind, listen)
void TcpSocket::setup() {
  resolveSocket();
  createSocket();
}

/// @brief Waits for a TCP connection and creates a new file descriptor that connection
/// @param conn_fd Stores the established connection descriptor
/// @param client_addr Client address info
/// @return TCP Socket Event (OK, TIMEOUT, TERMINATE)
TcpSocket::Events TcpSocket::acceptConnection(int& conn_fd,
                                              struct sockaddr_in& client_addr) {
  socklen_t client_addrlen = sizeof(client_addr);

  conn_fd = accept(socket_fd, reinterpret_cast<struct sockaddr*>(&client_addr),
                   &client_addrlen);
  if (conn_fd == -1) {
    if (terminateFlag.load()) {
      return TERMINATE;
    } else if (errno == EAGAIN || errno == EWOULDBLOCK) {
      return TIMEOUT;
    }
    throw AcceptTCPConnectionError();
  }
  return OK;
}

/// @brief Setup an individual TCP connection socket
/// @param conn_fd Connection descriptor
void TcpSocket::setupConnection(const int conn_fd) {
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

/// @brief Returns the socket address info
const addrinfo* TcpSocket::getSocketInfo() const { return socket_addr.get(); }
