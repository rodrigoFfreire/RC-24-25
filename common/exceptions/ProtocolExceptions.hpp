#ifndef COMMON_PROTOCOL_EXCEPTIONS_HPP
#define COMMON_PROTOCOL_EXCEPTIONS_HPP

#include "Exceptions.hpp"

class InvalidPacketException : public CommonException {
private:
    const std::string errorMsg = "Invalid packet received";

public:
    InvalidPacketException() : CommonException(errorMsg) {};
};

class ErrPacketException : public CommonException {
private:
    const std::string errorMsg = "This request caused an error";

public:
    ErrPacketException() : CommonException(errorMsg) {};
};

class UnexpectedPacketException : public CommonException {
private:
    const std::string errorMsg = "Unexpected packet received";

public:
    UnexpectedPacketException() : CommonException(errorMsg) {};
};

class PacketEncodingException : public CommonException {
private:
    const std::string errorMsg = "Cannot encode invalid packet data";

public:
    PacketEncodingException() : CommonException(errorMsg) {};
};

class ConnectionTimeoutError : public CommonError {
private:
    static constexpr const char* errorMsg = "TCP Connection timed out! ";

public:
    ConnectionTimeoutError() : CommonError(std::string(errorMsg)) {};
};

class ConnectionReadError : public CommonError {
private:
    static constexpr const char* errorMsg = "Failed to receive data from peer! ";

public:
    ConnectionReadError() : CommonError(std::string(errorMsg)) {};
};

class ConnectionWriteError : public CommonError {
private:
    static constexpr const char* errorMsg = "Failed to send data to peer! ";

public:
    ConnectionWriteError() : CommonError(std::string(errorMsg)) {};
};

class ConnectionResetError : public CommonError {
private:
    static constexpr const char* errorMsg = "Peer has reset the connection! ";

public:
    ConnectionResetError() : CommonError(std::string(errorMsg)) {};
};

#endif