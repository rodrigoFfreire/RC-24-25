#include "signals.hpp"

std::atomic<bool> terminateFlag(false);

void sig_handler(int signal) {
  (void)signal;
  terminateFlag = true;

}

void register_signal_handler() {
  struct sigaction sa;
  sa.sa_handler = &sig_handler;
  sa.sa_flags = 0;
  sigemptyset(&sa.sa_mask);

  if (sigaction(SIGINT, &sa, NULL) == -1) {
    throw SIGINTRegisterError();
  }
  if (sigaction(SIGUSR1, &sa, NULL) == -1) {
    throw SIGTERMRegisterError();
  }
}