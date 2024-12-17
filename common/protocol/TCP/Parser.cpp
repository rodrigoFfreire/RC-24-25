#include "./Parser.hpp"
#include "Parser.hpp"

/// @brief Parses a fixed size string and returns it
/// @param size expected size
std::string TcpParser::parseFixedString(size_t size) {
    std::string buffer(size, '\0');

    safe_read(connection_fd, &buffer[0], size);
    return buffer;
}

/// @brief Parses a fixed size numeric string and returns it
/// @param size expected size (number of digits)
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

/// @brief Parses a variable string with max size and end character delimeter
/// @param max_size string max size
/// @param end end delimiter
std::string TcpParser::parseVariableString(size_t max_size, char end) {
    size_t completed_bytes = 0;
    std::string buffer(max_size, '\0');

    while (max_size > 0) {
        ssize_t rd_bytes = safe_read(connection_fd, &(buffer[completed_bytes]), 1);

        if (std::isspace(buffer[completed_bytes])) {
            if (buffer[completed_bytes] != end) {
                throw InvalidPacketException();     // End delimiter was not present
            }
            buffer[completed_bytes] = '\0';
            break;
        }

        max_size -= static_cast<size_t>(rd_bytes);
        completed_bytes += rd_bytes;
    }

    return buffer;
}

/// @brief Confirms if the next character in the stream is equal to `c`
/// @param c 
void TcpParser::checkNextChar(const char c) {
    char _c;
    safe_read(connection_fd, &_c, sizeof(char));

    if (_c != c) {
        throw InvalidPacketException();
    }
}

/// @brief Parses a filename and returns it
std::string TcpParser::parseFileName() {
    return parseVariableString(FNAME_MAX, ' ');
}

/// @brief Confirms the next character is the argument delimeter (' ')
void TcpParser::next() {
    checkNextChar(' ');
}

/// @brief Confirms the next character is the end packet character ('\n')
void TcpParser::end() {
    checkNextChar('\n');
}

/// @brief Parses the Packet ID 
std::string TcpParser::parsePacketID() {
    return parseFixedString(PACKET_ID_LEN);
}

/// @brief Parses a packet status (3 chars)
std::string TcpParser::parseStatus() {
    return parseFixedString(STATUS_CODE_LEN);
}

/// @brief Parses a player ID 
std::string TcpParser::parsePlayerID() {
    std::string plID_str = parseFixedDigitString(PLID_LEN);
    try {
        int n = std::stoul(plID_str);
        if (n < 0 || n > PLID_MAX)
            throw InvalidPacketException();

        return plID_str;
    } catch (const std::invalid_argument& e) {
        throw InvalidPacketException();
    }
}

/// @brief Parses the file size argument and returns it
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

/// @brief Parses a file given its size
/// @param file_size Expected file size
/// @return Parsed file in string format
std::string TcpParser::parseFile(unsigned short file_size) {
    char buffer[FSIZE_MAX];

    ssize_t read_bytes = safe_read(connection_fd, buffer, file_size);
    return std::string(buffer, static_cast<size_t>(read_bytes));
}