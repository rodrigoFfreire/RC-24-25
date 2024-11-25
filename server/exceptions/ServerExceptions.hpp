#ifndef SERVER_EXCEPTIONS_HPP
#define SERVER_EXCEPTIONS_HPP

#include "../../common/exceptions/Exceptions.hpp"

class ServerReceiveError : public CommonError {
private:
    static constexpr const char* errorMsg = "Could not read data from client!";

public:
    ServerReceiveError() : CommonError(std::string(errorMsg) + std::strerror(errno)) {};
};

class ServerSendError : public CommonError {
private:
    static constexpr const char* errorMsg = "Could not send data to client!";

public:
    ServerSendError() : CommonError(std::string(errorMsg) + std::strerror(errno)) {};
};

class AcceptTCPConnectionError : public CommonError {
private:
    static constexpr const char* errorMsg = "Failed to accept TCP connection! ";

public:
    AcceptTCPConnectionError() : CommonError(std::string(errorMsg) + std::strerror(errno)) {};
};

class PeerNameResolveError : public CommonError {
private:
    static constexpr const char* errorMsg = "Failed to get peer's information! ";

public:
    PeerNameResolveError() : CommonError(std::string(errorMsg) + std::strerror(errno)) {};
};

class ConnectionTimeoutError : public CommonError {
private:
    static constexpr const char* errorMsg = "TCP Connection with client timed out! ";

public:
    ConnectionTimeoutError() : CommonError(std::string(errorMsg) + std::strerror(errno)) {};
};

class ClientResetError : public CommonError {
private:
    static constexpr const char* errorMsg = "TCP Connection reset by client! ";

public:
    ClientResetError() : CommonError(std::string(errorMsg) + std::strerror(errno)) {};
};

#endif