#ifndef SERVER_SIGNALS_HPP
#define SERVER_SIGNALS_HPP

#include <csignal>
#include "../../common/exceptions/SignalHandlerErrors.hpp"

extern volatile std::sig_atomic_t terminate_flag;

void sig_handler(int signal);
void register_signal_handler();


#endif