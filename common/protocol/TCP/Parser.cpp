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

std::string TcpParser::parseVariableString(size_t max_size, char end) {
    size_t completed_bytes = 0;
    std::string buffer(max_size, '\0');

    while (max_size > 0) {
        ssize_t rd_bytes = read(connection_fd, &(buffer[completed_bytes]), 1);

        if (rd_bytes < 0) {
          if (errno == EAGAIN || errno == EWOULDBLOCK) {
            throw ConnectionTimeoutError();
          } else if (errno == ECONNRESET) {
            throw ConnectionResetError();
          }
          throw ConnectionReadError();
        }
        else if (rd_bytes == 0)
          throw ConnectionResetError();

        if (std::isspace(buffer[completed_bytes])) {
            if (buffer[completed_bytes] != end) {
                throw InvalidPacketException();
            }
            buffer[completed_bytes] = '\0';
            break;
        }

        max_size -= (size_t)rd_bytes;
        completed_bytes += rd_bytes;
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

std::string TcpParser::parseFileName() {
    return parseVariableString(FNAME_MAX, ' ');
}

void TcpParser::next() {
    checkNextChar(' ');
}

void TcpParser::end() {
    checkNextChar('\n');
}

std::string TcpParser::parsePacketID() {
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

unsigned short TcpParser::parseFileSize() {
    std::string filesize_str = parseVariableString(FSIZE_STR_MAX, ' ');

    try {
        long fsize_long = std::stoul(filesize_str);
        if (fsize_long <= 0 || fsize_long > FSIZE_MAX) {
            throw std::out_of_range("");
        }
        if (fsize_long > 999) {
            checkNextChar('\n');
        }
        return static_cast<unsigned short>(fsize_long);
    } catch (const std::exception& e) {
        throw InvalidPacketException();
    }
}

std::string TcpParser::parseFile(unsigned short file_size) {
    char buffer[FSIZE_MAX];

    ssize_t read_bytes = safe_read(connection_fd, buffer, file_size);
    return std::string(buffer, read_bytes);
}