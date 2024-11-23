#ifndef COMMON_CONFIG_EXCEPTIONS_HPP
#define COMMON_CONFIG_EXCEPTIONS_HPP

#include "Exceptions.hpp"

class InvalidPortException : public CommonException {
private:
    const std::string errorMsg = "Port value must be an integer between 0-65535!";

public:
    InvalidPortException() : CommonException(errorMsg) {};
};

class InvalidIPAddressException : public CommonException {
private:
    const std::string errorMsg = "Invalid IPv4 server address!";

public:
    InvalidIPAddressException() : CommonException(errorMsg) {};
};

#endif