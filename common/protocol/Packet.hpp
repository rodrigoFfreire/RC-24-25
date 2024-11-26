#ifndef COMMON_PROTOCOL_PACKET_HPP
#define COMMON_PROTOCOL_PACKET_HPP

#include <sstream>
#include <string>
#include <cstdint>
#include "../constants.hpp"
#include "../exceptions/ProtocolExceptions.hpp"

class Packet {
public:
    virtual ~Packet() = default;
    virtual void decode(std::stringstream& packetStream) = 0;
    virtual std::string encode() const = 0;
};

class PacketParser {
private:
    std::stringstream& packetStream;

    std::string parseFixedString(size_t size);
    std::string parseFixedDigitString(size_t size);
    char parseColorChar();
    void checkNextChar(const char c);

public:
    PacketParser(std::stringstream& stream) : packetStream(stream) {};

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