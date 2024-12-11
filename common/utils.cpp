#include "utils.hpp"

ssize_t safe_read(int fd, char* buffer, size_t n) {
    ssize_t completed_bytes = 0;

    while (n > 0) {
        ssize_t rd_bytes = read(fd, buffer + completed_bytes, n);

        if (rd_bytes < 0) {
          if (errno == EAGAIN || errno == EWOULDBLOCK) {
            throw ConnectionTimeoutError();
          } else if (errno == ECONNRESET) {
            throw ConnectionResetError();
          }
          throw ConnectionReadError();
        }
        else if (rd_bytes == 0)
          throw ConnectionResetError();

        n -= (size_t)rd_bytes;
        completed_bytes += rd_bytes;
    }

    return completed_bytes;
}

ssize_t safe_write(int fd, const char* buffer, size_t n) {
  ssize_t completed_bytes = 0;

  while (n > 0) {
    ssize_t wr_bytes = write(fd, buffer + completed_bytes, n);

    if (wr_bytes < 0) {
      if (errno == EAGAIN || errno == EWOULDBLOCK) {
          throw ConnectionTimeoutError();
        } else if (errno == ECONNRESET) {
          throw ConnectionResetError();
        }
        throw ConnectionWriteError();
    }
    else if (wr_bytes == 0)
      break;

    n -= (size_t)wr_bytes;
    completed_bytes += wr_bytes;
  }

  return completed_bytes;
}

void formatTimestamp(std::ostringstream& ss, const time_t* tstamp, const std::string &format) {
  std::tm* timeInfo;
  
  if (tstamp == nullptr) {
    auto now = std::chrono::system_clock::now();
    time_t time_now = std::chrono::system_clock::to_time_t(now);
    timeInfo = std::localtime(&time_now);
  } else {
    timeInfo = std::localtime(tstamp);
  }

  if (timeInfo) {
    ss << std::put_time(timeInfo, format.c_str());
  }
}
