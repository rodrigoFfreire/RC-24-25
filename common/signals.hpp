#ifndef COMMON_SIGNALS_HPP
#define COMMON_SIGNALS_HPP

#include <csignal>
#include "exceptions/SignalHandlerErrors.hpp"

extern volatile sig_atomic_t terminate_flag;

void sig_handler(int signal);
void register_signal_handler();

#endif