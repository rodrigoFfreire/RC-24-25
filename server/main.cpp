#include "main.hpp"

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
        server.runUdp();
    } catch (const std::exception& e) {
        logger.log(Logger::Severity::ERROR, e.what(), true);
        return EXIT_FAILURE;
    }

    return 0;
}