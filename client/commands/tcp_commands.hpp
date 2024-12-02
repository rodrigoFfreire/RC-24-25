#ifndef CLIENT_COMMANDS_TCP_HPP
#define CLIENT_COMMANDS_TCP_HPP

#include "../Client.hpp"

void showTrialsHandler(GameState& state, TcpSocket& socket, std::stringstream& command_stream);

void showScoreboardHandler(GameState& state, TcpSocket& socket, std::stringstream& command_stream);

#endif