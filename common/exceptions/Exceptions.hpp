#ifndef COMMON_EXCEPTIONS_HPP
#define COMMON_EXCEPTIONS_HPP

#include <exception>
#include <string>
#include <cstring>
#include <stdexcept>
#include <cerrno>

class CommonError : public std::runtime_error {
public:
    explicit CommonError(const std::string& msg)
        : std::runtime_error(msg) {
    }
};

class CommonException : public std::exception {
protected:
    const std::string& _msg;

public:
    explicit CommonException(const std::string& msg)
        : _msg(msg) {
    };

    const char* what() const noexcept override {
        return _msg.c_str();
    };
};

#endif