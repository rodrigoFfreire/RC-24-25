#include "Logger.hpp"

std::string Logger::format(Severity level, const std::string &msg) {
    std::ostringstream ss;

    auto now = std::chrono::system_clock::now();
    time_t time = std::chrono::system_clock::to_time_t(now);

    ss << "[" << std::put_time(std::localtime(&time), TIMESTAMP_FORMAT) << "] ";
    ss << "[" << severityToStr(level) << "] ";
    //ss << "[" << entity << "]: ";
    ss << msg;

    return ss.str();
}

std::string Logger::severityToStr(Severity level) {
    switch (level) {
        case Severity::INFO: return LOGGER_LEVEL_INFO;
        case Severity::WARN: return LOGGER_LEVEL_WARNING;
        case Severity::ERROR: return LOGGER_LEVEL_ERROR;
        default: return "UNKNOWN";
    }
}

void Logger::log(Severity level, const std::string& msg, bool newLine) {
    std::lock_guard<std::mutex> lock(logMutex);

    std::string log_msg = format(level, msg);
    switch (level) {
        case Severity::INFO:
        case Severity::WARN:
            std::cout << log_msg;
            if (newLine) std::cout << std::endl;
            break;
        case Severity::ERROR:
            std::cerr << log_msg;
            if (newLine) std::cerr << std::endl;
            break;
    }
}

void Logger::logVerbose(Severity level, const std::string& msg, bool newLine) {
    if (isVerbose) {
        log(level, msg, newLine);
    }
}