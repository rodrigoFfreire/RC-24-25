#ifndef COMMON_PROTOCOL_TCP_PARSER_HPP
#define COMMON_PROTOCOL_TCP_PARSER_HPP

#include <sstream>
#include <string>
#include <cstdint>
#include <vector>
#include "../../constants.hpp"
#include "../../exceptions/ProtocolExceptions.hpp"
#include "../../utils.hpp"

class TcpParser {
private:
    int connection_fd;

    std::string parseFixedString(size_t size);
    std::string parseFixedDigitString(size_t size);
    std::string parseVariableString(size_t max_size);
    void checkNextChar(const char c);

public:
    TcpParser(int conn_fd) : connection_fd(conn_fd) {};

    void next();
    void end();
    std::string parsePacketID();
    std::string parseStatus();
    std::string parseFileName();
    std::string parseFile(unsigned short file_size);
    unsigned int parsePlayerID();
    unsigned short parseFileSize();
};


#endif