#ifndef SERVER_CONFIG_HPP
#define SERVER_CONFIG_HPP

#include <unistd.h>

#include <iostream>
#include <string>

#include "../../common/constants.hpp"
#include "../../common/exceptions/ConfigExceptions.hpp"

class Config {
 public:
  bool help = false;
  bool verbose = false;
  std::string port = DEFAULT_PORT;
  std::string fpath;
  std::string dataPath = DEFAULT_DATA_PATH;

  Config(int argc, char** argv);
  void setPort(const std::string& portStr);
  void setVerbose();
  void printUsage(std::ostream& s);
};

#endif