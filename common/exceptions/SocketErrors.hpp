#ifndef COMMON_SOCKET_ERRORS_HPP
#define COMMON_SOCKET_ERRORS_HPP

#include "Exceptions.hpp"
#include <netdb.h>

class SocketAddressResolveError : public CommonError {
private:
    static constexpr const char* errorMsg = "Failed to resolve socket address! ";

public:
    SocketAddressResolveError(int err) : CommonError(std::string(errorMsg) + gai_strerror(err)) {};
};

class SocketAlreadyCreatedError : public CommonError {
private:
    static constexpr const char* errorMsg = "Socket descriptor is already open! ";

public:
    SocketAlreadyCreatedError() : CommonError(std::string(errorMsg) + std::strerror(errno)) {};
};

class SocketOpenError : public CommonError {
private:
    static constexpr const char* errorMsg = "Failed to create socket descriptor! ";

public:
    SocketOpenError() : CommonError(std::string(errorMsg) + std::strerror(errno)) {};
};

class SocketBindError : public CommonError {
private:
    static constexpr const char* errorMsg = "Failed to bind socket! ";

public:
    SocketBindError() : CommonError(std::string(errorMsg) + std::strerror(errno)) {};
};

class SocketListenFailedError : public CommonError {
private:
    static constexpr const char* errorMsg = "Failed to start listening on socket! ";

public:
    SocketListenFailedError() : CommonError(std::string(errorMsg) + std::strerror(errno)) {};
};

class SocketSetOptFailedError : public CommonError {
private:
    static constexpr const char* errorMsg = "Failed to set socket options! ";

public:
    SocketSetOptFailedError() : CommonError(std::string(errorMsg) + std::strerror(errno)) {};
};


#endif