#ifndef COMMON_PROTOCOL_PACKET_HPP
#define COMMON_PROTOCOL_PACKET_HPP

#include <sstream>
#include <string>
#include <cctype>
#include "../constants.hpp"
#include "../exceptions/ProtocolExceptions.hpp"

class Packet {
public:
    virtual void decode(std::stringstream& packetStream) = 0;
    virtual std::string encode() const = 0;
};

class PacketParser {
private:
    std::stringstream& packetStream;

    std::string parseFixedString(size_t size);
    std::string parseFixedDigitString(size_t size);
    int parseInt();
    void checkNextChar(const char c);

public:
    PacketParser(std::stringstream& stream);

    void next();
    void end();
    std::string parsePacketID();
    u_int32_t parsePlayerID();
    u_int16_t parsePlayTime();
};

#endif