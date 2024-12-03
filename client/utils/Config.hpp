#ifndef CLIENT_UTILS_CONFIG_HPP
#define CLIENT_UTILS_CONFIG_HPP

#include <iostream>
#include <string>
#include <unistd.h>
#include "../../common/constants.hpp"
#include "../../common/exceptions/ConfigExceptions.hpp"

class Config {
public:
    bool help           = false;
    bool unicode        = false;
    std::string ipaddr  = DEFAULT_IPADRR;
    std::string port    = DEFAULT_PORT;
    std::string fpath;

    Config(int argc, char **argv);
    void setPort(const std::string& portStr);
    void setIP(const std::string& ipaddr);
    void setUnicode();
    void printUsage(std::ostream& s);  
};

#endif