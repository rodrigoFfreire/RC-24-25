#ifndef SERVER_TCP_SOCKET_HPP
#define SERVER_TCP_SOCKET_HPP

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
#include "../exceptions/ServerExceptions.hpp"

class TcpSocket {
 private:
  int socket_fd;
  std::string port;
  std::unique_ptr<struct addrinfo, decltype(&freeaddrinfo)> socket_addr;

  void createSocket();
  void resolveSocket();

 public:
  enum Events { OK, TIMEOUT, TERMINATE };

  TcpSocket(std::string port)
      : socket_fd(-1), port(port), socket_addr(nullptr, &freeaddrinfo) {};

  ~TcpSocket();

  void setup();
  TcpSocket::Events acceptConnection(int& conn_fd, struct sockaddr_in& client_addr);
  void setupConnection(const int conn_fd);
  const addrinfo* getSocketInfo() const;
};

#endif