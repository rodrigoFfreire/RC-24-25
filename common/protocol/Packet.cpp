#include "Packet.hpp"
#include <vector>

PacketParser::PacketParser(std::stringstream& stream)
    : packetStream(stream) {
    packetStream >> std::noskipws;  // Make stream not ignore whitespace
}

std::string PacketParser::parseFixedString(size_t size) {
    std::vector<char> buffer(size + 1);
    buffer[size] = '\0';
    char* data = buffer.data();

    packetStream.read(data, size);
    if (!packetStream || packetStream.gcount() != (long)size) {
        throw InvalidPacketException();
    }

    return std::string(data);
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
    if (!packetStream || next != c) {
        throw InvalidPacketException();
    }
    return;
}

char PacketParser::parseColorChar() {
    std::string colors = VALID_COLORS;
    char next;
    packetStream >> next;
    if (!packetStream || colors.find(next) == std::string::npos) {
        throw InvalidPacketException();
    }
    return next;
}

unsigned int PacketParser::parseUInt() {
    long n;
    packetStream >> n;
    if (!packetStream || n < 0 || n > UINT32_MAX) {
        throw InvalidPacketException();
    }
    return static_cast<unsigned int>(n);
}

void PacketParser::next() {
    checkNextChar(' ');
}

void PacketParser::end() {
    checkNextChar('\n');
    char a = -1;
    packetStream >> a;
    if (!packetStream.eof()) {
        throw InvalidPacketException();
    }
}

std::string PacketParser::parsePacketID() {
    std::string ID = parseFixedString(PACKET_ID_LEN);
    char c = packetStream.peek();
    if (c == -1) {
        throw InvalidPacketException();
    }
    return ID;
}

std::string PacketParser::parseStatus() {
    return parseFixedDigitString(STATUS_CODE_LEN);
}

unsigned int PacketParser::parsePlayerID() {
    std::string plID_str = parseFixedDigitString(PLAYER_ID_LEN);
    try {
        int n = std::stoul(plID_str);
        return static_cast<unsigned int>(n);
    } catch (const std::invalid_argument& e) {
        throw InvalidPacketException();
    }
}

char PacketParser::parseChar() {
    char next;
    packetStream >> next;
    if (!packetStream) {
        throw InvalidPacketException();
    }
    return next;
}

void PacketParser::parseKey(char *key) {
    for (int i = 0; i < SECRET_KEY_LEN; ++i) {
        this->next();
        key[i] = this->parseColorChar();
    }
}
