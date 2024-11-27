#include "tcp_commands.hpp"

#include "../../common/protocol/TCP/tcp.hpp"

void showTrialsHandler(const int& fd, Server& state, std::unique_ptr<TcpPacket>& replyPacket) {
    (void)fd; (void)state;
    ShowTrialsPacket request;
    auto reply = std::make_unique<ReplyShowTrialsPacket>();
    reply->status = ReplyShowTrialsPacket::FIN;

    request.read(fd);

    replyPacket = std::move(reply);
}

void showScoreboardHandler(const int& fd, Server& state, std::unique_ptr<TcpPacket>& replyPacket) {
    (void)fd; (void)state;
    ShowScoreboardPacket request;
    auto reply = std::make_unique<ReplyShowScoreboardPacket>();
    reply->status = ReplyShowScoreboardPacket::EMPTY;

    request.read(fd);

    replyPacket = std::move(reply);
}