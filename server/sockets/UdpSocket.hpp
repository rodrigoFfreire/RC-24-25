#ifndef SERVER_UDP_SOCKET_HPP
#define SERVER_UDP_SOCKET_HPP

#include <arpa/inet.h>
#include <netdb.h>
#include <sys/socket.h>
#include <unistd.h>

#include <csignal>
#include <memory>
#include <sstream>
#include <string>

#include "../../common/constants.hpp"
#include "../../common/exceptions/SocketErrors.hpp"
#include "../../common/protocol/UDP/udp.hpp"
#include "../exceptions/ServerExceptions.hpp"

class UdpSocket {
 private:
  int socket_fd;
  std::string port;
  std::unique_ptr<struct addrinfo, decltype(&freeaddrinfo)> socket_addr;

  void createSocket();
  void resolveSocket();

 public:
  enum Events { OK, TIMEOUT, TERMINATE };

  UdpSocket(std::string port)
      : socket_fd(-1), port(port), socket_addr(nullptr, &freeaddrinfo) {};

  ~UdpSocket();

  void setup();
  UdpSocket::Events receivePacket(std::stringstream& packetStream,
                                  struct sockaddr_in& client_addr);
  std::string sendPacket(std::unique_ptr<UdpPacket>& replyPacket,
                         struct sockaddr_in& client_addr);

  const struct addrinfo* getSocketInfo() const;
};

#endif