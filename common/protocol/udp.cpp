#include "udp.hpp"

void StartNewGamePacket::decode(std::stringstream &packetStream) {
    PacketParser parser(packetStream);

    playerID = parser.parsePlayerID();
    parser.next();
    time = parser.parsePlayTime();
    parser.end();
}

std::string StartNewGamePacket::encode() const {
    std::stringstream encoded_stream;
    encoded_stream << packetID << " " << playerID << " " << time << '\n';
    return encoded_stream.str();
}
