#ifndef COMMON_UTILS_HPP
#define COMMON_UTILS_HPP

#include <unistd.h>

ssize_t safe_read(int fd, char* buffer, size_t n);

ssize_t safe_write(int fd, const char* buffer, size_t n);

#endif