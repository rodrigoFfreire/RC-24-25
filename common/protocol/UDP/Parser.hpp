#ifndef COMMON_PROTOCOL_UDP_PARSER_HPP
#define COMMON_PROTOCOL_UDP_PARSER_HPP

#include <cstdint>
#include <sstream>
#include <string>

#include "../../constants.hpp"
#include "../../exceptions/ProtocolExceptions.hpp"

class UdpParser {
 private:
  std::stringstream& packetStream;

  std::string parseFixedString(ulong size);
  std::string parseFixedDigitString(ulong size);
  char parseColorChar();
  void checkNextChar(const char c);

 public:
  UdpParser(std::stringstream& stream) : packetStream(stream) {};

  void next();
  void end();
  std::string parsePacketID();
  std::string parseStatus();
  std::string parsePlayerID();
  unsigned int parseUInt();
  void parseKey(std::string& key);
};

#endif