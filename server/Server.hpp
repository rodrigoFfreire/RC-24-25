#ifndef SERVER_HPP
#define SERVER_HPP

#include <memory>
#include <string>
#include <unordered_map>

#include "../common/Logger.hpp"
#include "GameStore.hpp"
#include "sockets/TcpSocket.hpp"
#include "sockets/UdpSocket.hpp"
#include "utils/Config.hpp"
#include "utils/WorkerPool.hpp"

class Server {
  typedef void (*HandlerUdpFunc)(std::stringstream&, GameStore&, Logger&,
                                 std::unique_ptr<UdpPacket>&);
  typedef void (*HandlerTcpFunc)(const int, GameStore&, Logger&,
                                 std::unique_ptr<TcpPacket>&);

 private:
  std::string _port;
  UdpSocket _udpSocket;
  TcpSocket _tcpSocket;
  std::unordered_map<std::string, HandlerUdpFunc> _udp_handlers;
  std::unordered_map<std::string, HandlerTcpFunc> _tcp_handlers;
  WorkerPool _tcpPool;

  void registerCommands();
  void handleUdpCommand(const std::string& packetId, std::stringstream& packetStream,
                        std::unique_ptr<UdpPacket>& replyPacket);
  void handleTcpCommand(const std::string& packetId, const int conn_fd,
                        std::unique_ptr<TcpPacket>& replyPacket);

 public:
  Logger& logger;
  GameStore store;

  Server(Config& config, Logger& logger);
  void setupUdp();
  void setupTcp();
  void runUdp();
  void runTcp();
  void handleTcpConnection(const int conn_fd, const char* client_addrstr,
                           const sockaddr_in& client_addr);
};

#endif