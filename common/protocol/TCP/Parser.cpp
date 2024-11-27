#include "./Parser.hpp"
#include "Parser.hpp"

std::string TcpParser::parseFixedString(size_t size) {
    std::string buffer(size, '\0');

    safe_read(connection_fd, &buffer[0], size);
    return buffer;
}

std::string TcpParser::parseFixedDigitString(size_t size) {
    std::string buffer(size, '\0');

    safe_read(connection_fd, &buffer[0], size);

    for (char c: buffer) {
        if (!std::isdigit(static_cast<unsigned char>(c))) {
            throw InvalidPacketException();
        }
    }

    return buffer;
}

void TcpParser::checkNextChar(const char c) {
    char _c;
    safe_read(connection_fd, &_c, sizeof(char));

    if (_c != c) {
        throw InvalidPacketException();
    }
}

void TcpParser::next() {
    checkNextChar(' ');
}

void TcpParser::end() {
    checkNextChar('\n');
}

std::string TcpParser::parsePacketID()
{
    return parseFixedString(PACKET_ID_LEN);
}

std::string TcpParser::parseStatus() {
    return parseFixedString(STATUS_CODE_LEN);
}

unsigned int TcpParser::parsePlayerID() {
    std::string plID_str = parseFixedDigitString(PLAYER_ID_LEN);
    try {
        int n = std::stoul(plID_str);
        return static_cast<unsigned int>(n);
    } catch (const std::invalid_argument& e) {
        throw InvalidPacketException();
    }
}