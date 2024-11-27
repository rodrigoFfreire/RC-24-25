#ifndef COMMON_PROTOCOL_TCP_PARSER_HPP
#define COMMON_PROTOCOL_TCP_PARSER_HPP

#include <sstream>
#include <string>
#include <cstdint>
#include "../../constants.hpp"
#include "../../exceptions/ProtocolExceptions.hpp"
#include "../../utils.hpp"

class TcpParser {
private:
    int connection_fd;

    std::string parseFixedString(size_t size);
    std::string parseFixedDigitString(size_t size);
    void checkNextChar(const char c); 

public:
    TcpParser(int conn_fd) : connection_fd(conn_fd) {};

    void next();
    void end();
    std::string parsePacketID();
    std::string parseStatus();
    unsigned int parsePlayerID();
    unsigned int parseUInt() {return 0;};
};


#endif