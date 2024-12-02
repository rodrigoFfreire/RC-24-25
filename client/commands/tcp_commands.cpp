#include "tcp_commands.hpp"

void showTrialsHandler(GameState& state, TcpSocket& socket, std::stringstream& command_stream) {
    std::cout << "Showing trials..." << std::endl;
}

void showScoreboardHandler(GameState& state, TcpSocket& socket, std::stringstream& command_stream) {
    std::cout << "Showing scoreboard..." << std::endl;
}