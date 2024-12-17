#ifndef CLIENT_TCP_SOCKET_HPP
#define CLIENT_TCP_SOCKET_HPP

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
#include "../../common/protocol/TCP/tcp.hpp"
#include "../../common/utils.hpp"
#include "../exceptions/ClientExceptions.hpp"

class TcpSocket {
 private:
  int socket_fd;
  std::string& port;
  std::string& ipaddr;
  std::unique_ptr<struct addrinfo, decltype(&freeaddrinfo)> socket_addr;

  void createSocket();
  void resolveSocket();

 public:
  enum Events { OK, TIMEOUT, TERMINATE };

  TcpSocket(std::string& ipaddr, std::string& port)
      : socket_fd(-1), port(port), ipaddr(ipaddr), socket_addr(nullptr, &freeaddrinfo) {};

  ~TcpSocket();

  void setup();
  void end();
  void receivePacket(TcpPacket* packet);
  void sendPacket(TcpPacket* packet);
};

#endif