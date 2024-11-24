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

#endif