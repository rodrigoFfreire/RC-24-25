#include "Parser.hpp"

std::string UdpParser::parseFixedString(size_t size) {
    std::string buffer(size, '\0');
    char* data = buffer.data();

    packetStream.read(data, size);
    if (!packetStream || packetStream.gcount() != (long)size) {
        throw InvalidPacketException();
    }

    return std::string(data);
}

std::string UdpParser::parseFixedDigitString(size_t size) {
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

void UdpParser::checkNextChar(const char c) {
    char next;
    packetStream >> next;
    if (!packetStream || next != c) {
        throw InvalidPacketException();
    }
    return;
}

char UdpParser::parseColorChar() {
    std::string colors = VALID_COLORS;
    char next;
    packetStream >> next;
    if (!packetStream || colors.find(next) == std::string::npos) {
        throw InvalidPacketException();
    }
    return next;
}

unsigned int UdpParser::parseUInt() {
    long n;
    packetStream >> n;
    if (!packetStream || n < 0 || n > UINT32_MAX) {
        throw InvalidPacketException();
    }
    return static_cast<unsigned int>(n);
}

void UdpParser::next() {
    checkNextChar(' ');
}

void UdpParser::end() {
    checkNextChar('\n');
    char a = -1;
    packetStream >> a;
    if (!packetStream.eof()) {
        throw InvalidPacketException();
    }
}

std::string UdpParser::parsePacketID() {
    return parseFixedString(PACKET_ID_LEN);
}

std::string UdpParser::parseStatus() {
    return parseFixedString(STATUS_CODE_LEN);
}

unsigned int UdpParser::parsePlayerID() {
    std::string plID_str = parseFixedDigitString(PLAYER_ID_LEN);
    try {
        long n = std::stoul(plID_str);
        return static_cast<unsigned int>(n);
    } catch (const std::invalid_argument& e) {
        throw InvalidPacketException();
    }
}

char UdpParser::parseChar() {
    char next;
    packetStream >> next;
    if (!packetStream) {
        throw InvalidPacketException();
    }
    return next;
}

void UdpParser::parseKey(std::string& key) {
    for (int i = 0; i < SECRET_KEY_LEN; ++i) {
        this->next();
        key[i] = this->parseColorChar();
    }
}