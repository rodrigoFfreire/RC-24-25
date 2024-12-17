#include "UdpSocket.hpp"

#include <atomic>

extern std::atomic<bool> terminateFlag;

/// @brief Creates and binds the UDP socket
void UdpSocket::createSocket() {
  if (socket_fd != -1) {
    throw SocketAlreadyCreatedError();
  }

  // Open socket
  if ((socket_fd = socket(socket_addr->ai_family, socket_addr->ai_socktype,
                          socket_addr->ai_protocol)) == -1) {
    throw SocketOpenError();
  }

  // Set reusable address to avoid "already in use" errors
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
}

/// @brief Closes the socket
UdpSocket::~UdpSocket() {
  if (socket_fd != -1) {
    close(socket_fd);
    socket_fd = -1;
  }
}

/// @brief Resolves an usable address for the socket
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

/// @brief Sets up the socket (open, resolve, bind)
void UdpSocket::setup() {
  resolveSocket();
  createSocket();
}

/// @brief Receives a UDP packet
/// @param packetStream Stores the received packet in this stream
/// @param client_addr Client address info
/// @return UdpSocket Event (OK, TIMEOUT, TERMINATE)
UdpSocket::Events UdpSocket::receivePacket(std::stringstream& packetStream,
                                           struct sockaddr_in& client_addr) {
  char buffer[SOCK_BUFFER_SIZE];
  ssize_t received_bytes;
  socklen_t client_addrlen = sizeof(client_addr);

  received_bytes =
      recvfrom(socket_fd, buffer, sizeof(buffer), 0,
               reinterpret_cast<struct sockaddr*>(&client_addr), &client_addrlen);
  if (received_bytes == -1) {
    if (terminateFlag.load()) {
      return TERMINATE;
    } else if (errno == EAGAIN || errno == EWOULDBLOCK) {
      return TIMEOUT;
    }
    throw ServerReceiveError();
  }

  packetStream.write(buffer, received_bytes);
  return OK;
}

/// @brief Sends a UDP packet
/// @param replyPacket The pointer to the packet to be sent
/// @param client_addr Client address info
/// @return The sent serialiazed packet
std::string UdpSocket::sendPacket(std::unique_ptr<UdpPacket>& replyPacket,
                                  struct sockaddr_in& client_addr) {
  socklen_t client_addrlen = sizeof(client_addr);
  std::string packetStr = replyPacket->encode();
  const char* buffer = packetStr.c_str();
  if (sendto(socket_fd, buffer, packetStr.size(), 0,
             reinterpret_cast<struct sockaddr*>(&client_addr), client_addrlen) == -1) {
    throw ServerSendError();
  }

  return packetStr;
}

/// @brief Returns the socket address info
const addrinfo* UdpSocket::getSocketInfo() const { return socket_addr.get(); }
