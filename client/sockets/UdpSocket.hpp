#ifndef CLIENT_UDP_SOCKET_HPP
#define CLIENT_UDP_SOCKET_HPP

#include <arpa/inet.h>
#include <netdb.h>
#include <sys/socket.h>
#include <unistd.h>

#include <csignal>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>

#include "../../common/constants.hpp"
#include "../../common/exceptions/SocketErrors.hpp"
#include "../../common/protocol/UDP/udp.hpp"
#include "../exceptions/ClientExceptions.hpp"

class UdpSocket {
 private:
  int socket_fd;
  std::string& port;
  std::string& ipaddr;
  std::unique_ptr<struct addrinfo, decltype(&freeaddrinfo)> server_addr;

  void createSocket();
  void resolveSocket();

 public:
  UdpSocket(std::string& ipaddr, std::string& port)
      : socket_fd(-1), port(port), ipaddr(ipaddr), server_addr(nullptr, &freeaddrinfo) {};

  ~UdpSocket();

  void setup();
  void receivePacket(std::stringstream& packetStream);
  void sendPacket(UdpPacket* packet);
};

#endif