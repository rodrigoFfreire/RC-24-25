#include "UdpSocket.hpp"

extern volatile std::sig_atomic_t terminate_flag;

/// @brief Opens a new UDP socket
void UdpSocket::createSocket() {
  if (socket_fd != -1) {
    throw SocketAlreadyCreatedError();
  }

  // Open socket
  if ((socket_fd = socket(server_addr->ai_family, server_addr->ai_socktype,
                          server_addr->ai_protocol)) == -1) {
    throw SocketOpenError();
  }

  // Set reusable address to avoid "already in use" errors
  int yes = 1;
  if (setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) {
    throw SocketSetOptError();
  }

  // Set read/write timeout at socket level
  struct timeval tv_read;
  tv_read.tv_sec = CLIENT_RECV_TIMEOUT;
  tv_read.tv_usec = 0;
  if (setsockopt(socket_fd, SOL_SOCKET, SO_RCVTIMEO, &tv_read, sizeof(tv_read)) == -1) {
    throw SocketSetOptError();
  }

  struct timeval tv_write;
  tv_write.tv_sec = CLIENT_SEND_TIMEOUT;
  tv_write.tv_usec = 0;
  if (setsockopt(socket_fd, SOL_SOCKET, SO_RCVTIMEO, &tv_write, sizeof(tv_write)) == -1) {
    throw SocketSetOptError();
  }
}

/// @brief UdpSocket Destructor - Closes the socket
UdpSocket::~UdpSocket() {
  if (socket_fd != -1) {
    close(socket_fd);
    socket_fd = -1;
  }
}

/// @brief Resolves an usable address for the socket to connect to
void UdpSocket::resolveSocket() {
  int gai_err;
  struct addrinfo hints;
  struct addrinfo* raw_addrinfo = nullptr;

  // Create address hints
  memset(&hints, 0, sizeof(hints));
  hints.ai_family = AF_INET;
  hints.ai_socktype = SOCK_DGRAM;

  // Obtain usable address
  if ((gai_err = getaddrinfo(ipaddr.c_str(), port.c_str(), &hints, &raw_addrinfo)) != 0) {
    throw SocketAddressResolveError(gai_err);
  }
  server_addr.reset(raw_addrinfo);
}

/// @brief Sets up the client's UDP socket
void UdpSocket::setup() {
  resolveSocket();
  createSocket();
}

/// @brief Receives a UDP packet
/// @param packetStream Serialized packet stream
void UdpSocket::receivePacket(std::stringstream& packetStream) {
  char buffer[SOCK_BUFFER_SIZE];
  ssize_t received_bytes;
  socklen_t server_addrlen = sizeof(server_addr);

  addrinfo copy = *server_addr;
  received_bytes = recvfrom(socket_fd, buffer, sizeof(buffer), 0,
                            reinterpret_cast<sockaddr*>(&copy), &server_addrlen);
  if (received_bytes == -1) {
    if (errno == EAGAIN || errno == EWOULDBLOCK) {
      throw TimeoutError();
    }
    throw ClientReceiveError();
  }
  packetStream.write(buffer, received_bytes);
}

/// @brief Sends a UDP packet
/// @param packet UDP Packet object to be sent
void UdpSocket::sendPacket(UdpPacket* packet) {
  std::string packetStr = packet->encode();
  const char* buffer = packetStr.c_str();
  if (sendto(socket_fd, buffer, strlen(buffer), 0, server_addr->ai_addr,
             server_addr->ai_addrlen) == -1) {
    if (errno == EAGAIN || errno == EWOULDBLOCK) {
      throw TimeoutError();
    }
    throw ClientSendError();
  }
}
