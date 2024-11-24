#ifndef SERVER_COMMANDS_UDP_HPP
#define SERVER_COMMANDS_UDP_HPP

#include "../Server.hpp"

void startNewGameHandler(std::stringstream& packetStream, Server& state, std::unique_ptr<Packet>& replyPacket);

void tryHandler(std::stringstream& packetStream, Server& state, std::unique_ptr<Packet>& replyPacket);

void quitHandler(std::stringstream& packetStream, Server& state, std::unique_ptr<Packet>& replyPacket);

void debugGameHandler(std::stringstream& packetStream, Server& state, std::unique_ptr<Packet>& replyPacket);


#endif