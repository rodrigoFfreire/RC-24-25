#ifndef COMMON_LOGGER_HPP
#define COMMON_LOGGER_HPP

#include <iostream>
#include <mutex>
#include <sstream>

#include "constants.hpp"
#include "utils.hpp"

#define LOGGER_LEVEL_INFO "INFO"
#define LOGGER_LEVEL_WARNING "WARN"
#define LOGGER_LEVEL_ERROR "ERR"

class Logger {
 public:
  enum class Severity { INFO, WARN, ERROR };

  void log(Severity level, const std::string& msg, bool newLine = false);
  void logVerbose(Severity level, const std::string& msg, bool newLine = false);
  void setVerbose(bool verbose) {
    isVerbose = verbose;
    if (isVerbose) std::cout << "Running in verbose mode\n";
  }

 private:
  bool isVerbose;
  std::mutex logMutex;

  std::string format(Severity level, const std::string& msg);

  std::string severityToStr(Severity level);
};

#endif