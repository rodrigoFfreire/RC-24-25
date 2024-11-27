#ifndef COMMON_PROTOCOL_UDP_PARSER_HPP
#define COMMON_PROTOCOL_UDP_PARSER_HPP

#include <sstream>
#include <string>
#include <cstdint>
#include "../../constants.hpp"
#include "../../exceptions/ProtocolExceptions.hpp"

class UdpParser {
private:
    std::stringstream& packetStream;

    std::string parseFixedString(size_t size);
    std::string parseFixedDigitString(size_t size);
    char parseColorChar();
    void checkNextChar(const char c);

public:
    UdpParser(std::stringstream& stream) : packetStream(stream) {};

    void next();
    void end();
    std::string parsePacketID();
    std::string parseStatus();
    unsigned int parsePlayerID();
    unsigned int parseUInt();
    char parseChar();
    void parseKey(char *key);
};

#endif