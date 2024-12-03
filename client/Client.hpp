#ifndef CLIENT_CLIENT_HPP
#define CLIENT_CLIENT_HPP

#include <unordered_map>
#include <string>
#include <sstream>
#include "GameState.hpp"
#include "utils/Config.hpp"
#include "../common/exceptions/SocketErrors.hpp"
#include "sockets/TcpSocket.hpp"
#include "sockets/UdpSocket.hpp"

class Client {
    typedef void (*HandlerUdpFunc)(GameState&, UdpSocket&, std::stringstream&);
    typedef void (*HandlerTcpFunc)(GameState&, TcpSocket&);

private:
    TcpSocket tcp_socket;
    UdpSocket udp_socket;
    std::unordered_map<std::string, HandlerUdpFunc> udp_handlers;
    std::unordered_map<std::string, HandlerTcpFunc> tcp_handlers;
    GameState game_state;

    void registerCommands();
    void handleCommand(std::string& command, std::stringstream& command_stream);

public:
    bool exit = false;

    Client(Config& config);
    void getNextCommand();
    void printHelp();
};

#endif