#ifndef COMMON_EXCEPTIONS_HPP
#define COMMON_EXCEPTIONS_HPP

#include <cerrno>
#include <cstring>
#include <exception>
#include <stdexcept>
#include <string>

class CommonError : public std::runtime_error {
 public:
  explicit CommonError(const std::string& msg) : std::runtime_error(msg) {}
};

class CommonException : public std::exception {
 protected:
  const std::string& _msg;

 public:
  explicit CommonException(const std::string& msg) : _msg(msg) {};

  const char* what() const noexcept override { return _msg.c_str(); };
};

#endif