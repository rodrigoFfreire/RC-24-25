#include "tcp_commands.hpp"

#include "../../common/protocol/Packet.hpp"
#include "../../common/protocol/tcp.hpp"

void showTrialsHandler(const int& fd, Server& state, std::unique_ptr<Packet>& replyPacket) {
    auto reply = std::make_unique<ReplyShowTrialsPacket>();
    replyPacket = std::move(reply);
}

void showScoreboardHandler(const int& fd, Server& state, std::unique_ptr<Packet>& replyPacket) {
    auto reply = std::make_unique<ReplyShowScoreboardPacket>();
    replyPacket = std::move(reply);
}