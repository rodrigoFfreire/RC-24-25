#include "tcp_commands.hpp"

void showTrialsHandler(GameState& state, TcpSocket& socket, std::stringstream& command_stream) {
    ShowTrialsPacket request;
    ReplyShowTrialsPacket reply;

    try {
        request.playerID = state.getPlid();
        socket.sendPacket(&request);
        socket.receivePacket(&request);

        switch (reply.status) {
        case ReplyShowTrialsPacket::ACT:
        case ReplyShowTrialsPacket::FIN:
            state.saveFile(reply.fname, reply.fdata);
            break;
        case ReplyShowTrialsPacket::NOK:
        default:
            throw BadCommandException();
        }
    } catch (const CommonException& e) {
        std::cout << e.what() << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "ERROR: " << e.what() << std::endl;
    }
}

void showScoreboardHandler(GameState& state, TcpSocket& socket, std::stringstream& command_stream) {
    std::cout << "Showing scoreboard..." << std::endl;
}