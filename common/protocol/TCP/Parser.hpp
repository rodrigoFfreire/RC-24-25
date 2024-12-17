#ifndef COMMON_PROTOCOL_TCP_PARSER_HPP
#define COMMON_PROTOCOL_TCP_PARSER_HPP

#include <cstdint>
#include <sstream>
#include <string>
#include <vector>

#include "../../constants.hpp"
#include "../../exceptions/ProtocolExceptions.hpp"
#include "../../utils.hpp"

class TcpParser {
 private:
  int connection_fd;

  std::string parseFixedString(size_t size);
  std::string parseFixedDigitString(size_t size);
  std::string parseVariableString(size_t max_size, char end);

 public:
  TcpParser(int conn_fd) : connection_fd(conn_fd) {};

  void next();
  void end();
  void checkNextChar(const char c);
  std::string parsePacketID();
  std::string parseStatus();
  std::string parseFileName();
  std::string parseFile(unsigned short file_size);
  std::string parsePlayerID();
  unsigned short parseFileSize();
};

#endif