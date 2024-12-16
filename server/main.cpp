#include <thread>

#include "Server.hpp"
#include "utils/signals.hpp"
#include "utils/Config.hpp"
#include "../common/Logger.hpp"

int main(int argc, char **argv) {
    Logger logger;

    try {
        Config config(argc, argv);
        if (config.help) {
            return EXIT_SUCCESS;
        }
        logger.setVerbose(config.verbose);

        register_signal_handler();

        Server server(config, logger);
        server.setupUdp();
        server.setupTcp();

        std::thread udpThread(&Server::runUdp, &server);
        std::thread tcpThread(&Server::runTcp, &server);

        udpThread.join();
        tcpThread.join();
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
