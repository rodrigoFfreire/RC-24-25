#ifndef CLIENT_COMMANDS_TCP_HPP
#define CLIENT_COMMANDS_TCP_HPP

#include "../Client.hpp"
#include "../exceptions/CommandExceptions.hpp"

void showTrialsHandler(GameState& state, TcpSocket& socket);

void showScoreboardHandler(GameState& state, TcpSocket& socket);

#endif