#include "utils.hpp"

ssize_t safe_read(int fd, char* buffer, size_t n) {
    ssize_t completed_bytes = 0;

    while (n > 0) {
        ssize_t rd_bytes = read(fd, buffer + completed_bytes, n);

        if (rd_bytes < 0)
            return -1;
        else if (rd_bytes == 0)
            break;

        n -= (size_t)rd_bytes;
        completed_bytes += rd_bytes;
    }

    return completed_bytes;
}

ssize_t safe_write(int fd, const char* buffer, size_t n) {
  ssize_t completed_bytes = 0;

  while (n > 0) {
    ssize_t wr_bytes = write(fd, buffer + completed_bytes, n);

    if (wr_bytes < 0)
      return -1;
    else if (wr_bytes == 0)
      break;

    n -= (size_t)wr_bytes;
    completed_bytes += wr_bytes;
  }

  return completed_bytes;
}