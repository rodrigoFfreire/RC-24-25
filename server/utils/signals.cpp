#include "signals.hpp"

// Global flag that controls if the server needs to be terminated
std::atomic<bool> terminateFlag(false);

// Signal handler function that set terminate flag to true
void sig_handler(int signal) {
  (void)signal;
  terminateFlag = true;
}

/// @brief Registers the above signal handler for SIGINT and SIGTERM
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