#ifndef CLIENT_COMMANDS_UDP_HPP
#define CLIENT_COMMANDS_UDP_HPP

#include "../Client.hpp"
#include "../exceptions/CommandExceptions.hpp"

void startNewGameHandler(GameState& state, UdpSocket& socket,
                         std::stringstream& command_stream);

void tryHandler(GameState& state, UdpSocket& socket, std::stringstream& command_stream);

void quitHandler(GameState& state, UdpSocket& socket, std::stringstream& command_stream);

void debugGameHandler(GameState& state, UdpSocket& socket,
                      std::stringstream& command_stream);

#endif