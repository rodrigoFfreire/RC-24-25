#ifndef SERVER_COMMANDS_TCP_HPP
#define SERVER_COMMANDS_TCP_HPP

#include "../GameStore.hpp"
#include "../../common/Logger.hpp"
#include "../../common/protocol/TCP/tcp.hpp"

void showTrialsHandler(const int fd, GameStore& store, Logger& logger, std::unique_ptr<TcpPacket>& replyPacket);

void showScoreboardHandler(const int fd, GameStore& store, Logger& logger, std::unique_ptr<TcpPacket>& replyPacket);


#endif