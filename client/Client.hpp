#ifndef CLIENT_CLIENT_HPP
#define CLIENT_CLIENT_HPP

#include "utils/Config.hpp"

class Client {
private:
    int udp_fd;
    int tcp_fd;
};

#endif