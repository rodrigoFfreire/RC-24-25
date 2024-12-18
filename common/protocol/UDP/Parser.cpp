#include "Parser.hpp"

/// @brief Parses a fixed size string and returns it
/// @param size expected size
std::string UdpParser::parseFixedString(ulong size) {
  std::string buffer(size, '\0');
  char* data = buffer.data();

  packetStream.read(data, size);
  if (!packetStream || packetStream.gcount() != static_cast<long>(size)) {
    throw InvalidPacketException();
  }

  return std::string(data);
}

/// @brief Parses a fixed size numeric string and returns it
/// @param size expected size (number of digits)
std::string UdpParser::parseFixedDigitString(ulong size) {
  std::string str(size, '\0');
  for (ulong i = 0; i < size; ++i) {
    char c;
    packetStream >> c;
    if (!packetStream.good() || !std::isdigit(c)) {
      throw InvalidPacketException();
    }
    str[i] = c;
  }
  return str;
}

/// @brief Confirms if the next character in the stream is equal to `c`
/// @param c
void UdpParser::checkNextChar(const char c) {
  char next;
  packetStream >> next;
  if (!packetStream || next != c) {
    throw InvalidPacketException();
  }
  return;
}

/// @brief Parses a color character and returns it
char UdpParser::parseColorChar() {
  std::string colors = VALID_COLORS;
  char next;
  packetStream >> next;
  if (!packetStream || colors.find(next) == std::string::npos) {
    throw InvalidPacketException();
  }
  return next;
}

/// @brief Parses an unsigned int value and returns it
unsigned int UdpParser::parseUInt() {
  long n;
  packetStream >> n;
  if (!packetStream || n < 0 || n > UINT32_MAX) {
    throw InvalidPacketException();
  }
  return static_cast<unsigned int>(n);
}

/// @brief Confirms the next character is the argument delimeter (' ')
void UdpParser::next() { checkNextChar(' '); }

/// @brief Confirms the next character is the end packet character ('\n')
void UdpParser::end() {
  checkNextChar('\n');
  char a = -1;
  packetStream >> a;
  if (!packetStream.eof()) {
    throw InvalidPacketException();
  }
}

/// @brief Parses the Packet ID
std::string UdpParser::parsePacketID() { return parseFixedString(PACKET_ID_LEN); }

/// @brief Parses a packet status (3 chars)
std::string UdpParser::parseStatus() { return parseFixedString(STATUS_CODE_LEN); }

/// @brief Parses a player ID
std::string UdpParser::parsePlayerID() {
  std::string plID_str = parseFixedDigitString(PLID_LEN);
  try {
    ulong n = std::stoul(plID_str);
    if (n > PLID_MAX) throw InvalidPacketException();

    return plID_str;
  } catch (const std::invalid_argument& e) {
    throw InvalidPacketException();
  }
}

/// @brief Parses an attempt key
/// @param key Stores the parsed key here
void UdpParser::parseKey(std::string& key) {
  for (size_t i = 0; i < SECRET_KEY_LEN; ++i) {
    this->next();
    key[i] = this->parseColorChar();
  }
}