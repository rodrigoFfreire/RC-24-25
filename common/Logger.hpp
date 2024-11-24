#ifndef COMMON_LOGGER_HPP
#define COMMON_LOGGER_HPP

#include <mutex>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <chrono>
#include "constants.hpp"

class Logger {
public:
    enum class Severity { INFO, WARN, ERROR };

    void log(Severity level, const std::string& msg, bool newLine = false);
    void logVerbose(Severity level, const std::string& msg, bool newLine = false);
    void setVerbose(bool verbose) {
        isVerbose = verbose;
        if (isVerbose)
            std::cout << "Running in verbose mode\n";
    }

private:
    bool isVerbose;
    std::mutex logMutex;

    std::string format(Severity level, const std::string& msg);

    std::string severityToStr(Severity level);

};

#endif