#ifndef COMMON_PROTOCOL_EXCEPTIONS_HPP
#define COMMON_PROTOCOL_EXCEPTIONS_HPP

#include "Exceptions.hpp"

class InvalidPacketException : public CommonException {
private:
    const std::string errorMsg = "Invalid packet received";

public:
    InvalidPacketException() : CommonException(errorMsg) {};
};

class UnexpectedPacketException : public CommonException {
private:
    const std::string errorMsg = "Unexpected packet received";

public:
    UnexpectedPacketException() : CommonException(errorMsg) {};
};

#endif