#include "main.hpp"

int main(int argc, char **argv) {
   try {
        Config config(argc, argv);
        if (config.help) {
            return EXIT_SUCCESS;
        }

        // do stuff
   } catch (...) {
        return EXIT_FAILURE;
   }

   return EXIT_SUCCESS;
}