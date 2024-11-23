#include "Packet.hpp"

PacketParser::PacketParser(std::stringstream& stream)
    : packetStream(stream) {
    packetStream >> std::noskipws;  // Make stream not ignore whitespace
}

std::string PacketParser::parseFixedString(size_t size) {
    std::string str(size, '\0');
    packetStream >> str;

    if (!packetStream || packetStream.gcount() < size) {
        throw InvalidPacketException();
    }
    return str;
}

std::string PacketParser::parseFixedDigitString(size_t size) {
    std::string str(size, '\0');
    for (size_t i = 0; i < size; ++i) {
        char c;
        packetStream >> c;
        if (!packetStream.good() || !std::isdigit(c)) {
            throw InvalidPacketException();
        }
        str[i] = c;
    }
    return str;
}

void PacketParser::checkNextChar(const char c) {
    char next;
    packetStream >> next;
    if (!packetStream.good() || next != c) {
        throw InvalidPacketException();
    }
    return;
}

int PacketParser::parseInt() {
    int n;
    packetStream >> n;
    if (packetStream.fail()) {
        throw InvalidPacketException();
    }
    return n;
}

void PacketParser::next() {
    checkNextChar(' ');
}

void PacketParser::end() {
    checkNextChar('\n');
}

std::string PacketParser::parsePacketID() {
    return parseFixedString(PACKET_ID_LEN);
}

u_int32_t PacketParser::parsePlayerID() {
    std::string plID_str = parseFixedDigitString(PLAYER_ID_LEN);
    try {
        int n = std::stoi(plID_str);
        return static_cast<u_int32_t>(n);
    } catch (const std::invalid_argument& e) {
        throw InvalidPacketException();
    }
}

u_int16_t PacketParser::parsePlayTime() {
    int time = parseInt();
    if (time < 0 || time > 999) {
        throw InvalidPacketException();
    }
    return static_cast<u_int16_t>(time);
}