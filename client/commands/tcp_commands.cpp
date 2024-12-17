#include "tcp_commands.hpp"

/// @brief Show trials handler. Sends the appropriate request and displays the last played
/// or current game
/// @param state Game state
/// @param socket TcpSocket object
void showTrialsHandler(GameState& state, TcpSocket& socket) {
  ShowTrialsPacket request;
  ReplyShowTrialsPacket reply;

  try {
    request.playerID = state.getPlid();

    socket.setup();

    socket.sendPacket(&request);
    socket.receivePacket(&reply);

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

  socket.end();
}

/// @brief Show scoreboard handler. Sends the appropriate request and displays the
/// scoreboard
/// @param state Game state
/// @param socket TcpSocket object
void showScoreboardHandler(GameState& state, TcpSocket& socket) {
  ShowScoreboardPacket request;
  ReplyShowScoreboardPacket reply;

  try {
    socket.setup();

    socket.sendPacket(&request);
    socket.receivePacket(&reply);

    switch (reply.status) {
      case ReplyShowScoreboardPacket::OK:
        state.saveFile(reply.fname, reply.fdata);
        break;
      case ReplyShowScoreboardPacket::EMPTY:
        throw EmptyScoreboardException();
        break;
      default:
        throw BadCommandException();
    }
  } catch (const CommonException& e) {
    std::cout << e.what() << std::endl;
  } catch (const std::exception& e) {
    std::cerr << "ERROR: " << e.what() << std::endl;
  }

  socket.end();
}