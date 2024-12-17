#ifndef COMMON_UTILS_HPP
#define COMMON_UTILS_HPP

#include <unistd.h>

#include <chrono>
#include <iomanip>
#include <sstream>

#include "exceptions/ProtocolExceptions.hpp"

ssize_t safe_read(int fd, char* buffer, size_t n);

ssize_t safe_write(int fd, const char* buffer, size_t n);

void formatTimestamp(std::ostringstream& ss, const std::time_t* tstamp,
                     const std::string& format);

#endif