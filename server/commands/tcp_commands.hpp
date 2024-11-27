#ifndef SERVER_COMMANDS_TCP_HPP
#define SERVER_COMMANDS_TCP_HPP

#include "../Server.hpp"

void showTrialsHandler(const int& fd, Server& state, std::unique_ptr<TcpPacket>& replyPacket);

void showScoreboardHandler(const int& fd, Server& state, std::unique_ptr<TcpPacket>& replyPacket);


#endif