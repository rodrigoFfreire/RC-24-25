#include "signals.hpp"

volatile std::sig_atomic_t terminate_flag = 0;

void sig_handler(int signal) {
  (void)signal;
  terminate_flag = 1;
}

void register_signal_handler() {
  struct sigaction sa;
  sa.sa_handler = &sig_handler;
  sa.sa_flags = 0;
  sigemptyset(&sa.sa_mask);

  if (sigaction(SIGINT, &sa, NULL) == -1) {
    throw SIGINTRegisterError();
  }
  if (sigaction(SIGTERM, &sa, NULL) == -1) {
    throw SIGTERMRegisterError();
  }
}