#ifndef SERVER_SIGNALS_HPP
#define SERVER_SIGNALS_HPP

#include <atomic>
#include <csignal>

#include "../../common/exceptions/SignalHandlerErrors.hpp"

extern std::atomic<bool> terminateFlag;

void sig_handler(int signal);
void register_signal_handler();

#endif