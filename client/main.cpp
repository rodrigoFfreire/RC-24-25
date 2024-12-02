#include "main.hpp"

int main(int argc, char **argv) {
   try {
        Config config(argc, argv);
        if (config.help) {
            return EXIT_SUCCESS;
        }

        Client client(config);
        while (!client.exit) {
            client.getNextCommand();
        }
        // do stuff
   } catch (const std::exception& e) {
        std::cout << "ERROR: " << e.what() << std::endl;
   }

   return EXIT_SUCCESS;
}