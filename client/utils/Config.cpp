#include "Config.hpp"

/// @brief Creates the client configuration object using argv
/// @param argc 
/// @param argv 
Config::Config(int argc, char** argv) {
  int opt;
  fpath = std::string(argv[0]);

  while ((opt = getopt(argc, argv, "n:p:uh")) != -1) {
    switch (opt) {
      case 'p':
        setPort(std::string(optarg));
        break;

      case 'n':
        setIP(std::string(optarg));
        break;

      case 'u':
        setUnicode();
        break;

      case 'h':
        help = true;
        printUsage(std::cout);
        return;
        break;

      default:
        this->printUsage(std::cerr);
        throw std::invalid_argument("");
    }
  }
}

/// @brief Sets the target IP address
/// @param ip_addr IP address (string format)
void Config::setIP(const std::string& ip_addr) {
  ipaddr = ip_addr;
}

/// @brief Sets unicode mode (Renders colors with emojis)
void Config::setUnicode() {
  unicode = true;
}

/// @brief Sets the target Port
/// @param port_str Port (string format)
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

/// @brief Prints the player usage
/// @param s Output stream
void Config::printUsage(std::ostream& s) {
  s << "Usage: " << this->fpath << " [-n <GSIP>] [-p <GSport>] [-u] [-h]" << std::endl;
  s << "Options:" << std::endl;
  s << "\t-n <GSIP>\t Sets Game server IP address" << std::endl;
  s << "\t-p <GSport>\t Sets Game server port" << std::endl;
  s << "\t-u\t\t Unicode mode. Uses emojis to render the colors (Recommended)" << std::endl;
  s << "\t-h\t\t Displays this usage message" << std::endl;
}