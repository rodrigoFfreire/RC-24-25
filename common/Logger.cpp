#include "Logger.hpp"

/// @brief Formats a log message with the current date, time and severity color
/// @param level Severity level (INFO, WARN, ERROR)
/// @param msg Log message
/// @return Formatted log message
std::string Logger::format(Severity level, const std::string &msg) {
    std::ostringstream ss;

    switch (level) {
    case Severity::WARN:
        ss << "\033[1;33m";
        break;
    case Severity::ERROR:
        ss << "\033[1;31m";
        break;
    case Severity::INFO:
    default:
        break;
    }

    ss << "[";
    formatTimestamp(ss, nullptr, TSTAMP_DATE_TIME_PRETTY);
    ss << "] ";
    ss << "[" << severityToStr(level) << "] ";
    ss << msg << "\033[0m";

    return ss.str();
}

/// @brief Severity level to string representation
/// @param level Severity level
std::string Logger::severityToStr(Severity level) {
    switch (level) {
        case Severity::INFO: return LOGGER_LEVEL_INFO;
        case Severity::WARN: return LOGGER_LEVEL_WARNING;
        case Severity::ERROR: return LOGGER_LEVEL_ERROR;
        default: return "UNKNOWN";
    }
}

/// @brief Creates the formatted log message and outputs it to the correct stream
/// @param level Severity level
/// @param msg Log message
/// @param newLine If true adds a `\n` to the message
void Logger::log(Severity level, const std::string& msg, bool newLine) {
    // Mutex is to ensure multiple threads write to the output streams without causing data races
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
        default:
            break;
    }
}

/// @brief Creates the formatted log message and outputs it if verbose mode is set
/// @param level Severity level
/// @param msg Log message
/// @param newLine If true adds a `\n` to the message
void Logger::logVerbose(Severity level, const std::string& msg, bool newLine) {
    if (isVerbose) {
        log(level, msg, newLine);
    }
}