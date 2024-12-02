#ifndef CLIENT_EXCEPTIONS_HPP
#define CLIENT_EXCEPTIONS_HPP

#include "../../common/exceptions/Exceptions.hpp"

class ClientReceiveError : public CommonError {
private:
    static constexpr const char* errorMsg = "Could not read data from server! ";

public:
    ClientReceiveError() : CommonError(std::string(errorMsg) + std::strerror(errno)) {};
};

class ClientSendError : public CommonError {
private:
    static constexpr const char* errorMsg = "Could not send data to server! ";

public:
    ClientSendError() : CommonError(std::string(errorMsg) + std::strerror(errno)) {};
};

class ConnectTCPError : public CommonError {
private:
    static constexpr const char* errorMsg = "Failed to connect to server over TCP! ";

public:
    ConnectTCPError() : CommonError(std::string(errorMsg) + std::strerror(errno)) {};
};

class TimeoutError : public CommonError {
private:
    static constexpr const char* errorMsg = "Failed to communicate with server within reasonable time!";

public:
    TimeoutError() : CommonError(std::string(errorMsg) + std::strerror(errno)) {};
};

#endif