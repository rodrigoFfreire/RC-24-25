#include <thread>

#include "../common/Logger.hpp"
#include "Server.hpp"
#include "utils/Config.hpp"
#include "utils/signals.hpp"

int main(int argc, char** argv) {
  Logger logger;

  try {
    // Obtain server configuration from argv
    Config config(argc, argv);
    if (config.help) {
      return EXIT_SUCCESS;
    }
    logger.setVerbose(config.verbose);

    register_signal_handler();

    // Create server, setup sockets
    Server server(config, logger);
    server.setupUdp();
    server.setupTcp();

    // Run each listener in separate threads
    std::thread udpThread(&Server::runUdp, &server);
    std::thread tcpThread(&Server::runTcp, &server);

    // Waits for these threads to finish
    udpThread.join();
    tcpThread.join();
  } catch (const std::exception& e) {
    std::cerr << e.what() << std::endl;
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
