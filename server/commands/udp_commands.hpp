#ifndef SERVER_COMMANDS_UDP_HPP
#define SERVER_COMMANDS_UDP_HPP

#include "../../common/protocol/UDP/udp.hpp"
#include "../GameStore.hpp"
#include "../../common/Logger.hpp"

void startNewGameHandler(std::stringstream& packetStream, GameStore& store, Logger& logger, std::unique_ptr<UdpPacket>& replyPacket);

void tryHandler(std::stringstream& packetStream, GameStore& store, Logger& logger, std::unique_ptr<UdpPacket>& replyPacket);

void quitHandler(std::stringstream& packetStream, GameStore& store, Logger& logger, std::unique_ptr<UdpPacket>& replyPacket);

void debugGameHandler(std::stringstream& packetStream, GameStore& store, Logger& logger, std::unique_ptr<UdpPacket>& replyPacket);


#endif