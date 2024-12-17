#include "Config.hpp"

/// @brief Creates the server configuration object using argv
/// @param argc 
/// @param argv 
Config::Config(int argc, char** argv) {
  int opt;
  this->fpath = std::string(argv[0]);

  while ((opt = getopt(argc, argv, "p:vh")) != -1) {
    switch (opt) {
      case 'p':
        this->setPort(std::string(optarg));
        break;

      case 'v':
        this->setVerbose();
        break;

      case 'h':
        help = true;
        this->printUsage(std::cout);
        return;
        break;

      default:
        this->printUsage(std::cerr);
        throw std::invalid_argument("");
    }
  }
}

/// @brief Sets verbose mode on
void Config::setVerbose() {
  this->verbose = true;
}

/// @brief Sets configured port
/// @param port_str Port in string format
void Config::setPort(const std::string& port_str) {
  try {
    long port_value = std::stol(port_str);

    if (port_value < 0 || port_value > 65535) {
      throw std::out_of_range("");
    }

    this->port = port_str;
  } catch (const std::exception& e) {
    throw InvalidPortException();
  }
}

/// @brief Prints the GS usage
/// @param s Output stream
void Config::printUsage(std::ostream& s) {
  s << "Usage: " << this->fpath << " [-p <GSport>] [-v] [-h]" << std::endl;
  s << "Options:" << std::endl;
  s << "\t-p <GSport>\t Sets Game server port" << std::endl;
  s << "\t-v\t\t Enables verbose mode" << std::endl;
  s << "\t-h\t\t Displays this usage message" << std::endl;
}