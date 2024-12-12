#include "tcp_commands.hpp"

#include "../../common/protocol/TCP/tcp.hpp"

void showTrialsHandler(const int& fd, Server& state, std::unique_ptr<TcpPacket>& replyPacket) {
    ShowTrialsPacket request;
    auto reply = std::make_unique<ReplyShowTrialsPacket>();
    reply->status = ReplyShowTrialsPacket::NOK;

    try {
        time_t now = state.getCommandTime();
        request.read(fd);

        std::ostringstream oss;
        oss << std::setw(PLAYER_ID_LEN) << std::setfill('0') << request.playerID;
        std::string plid = oss.str();
        std::string file_str;

        Game::Status status = state.store.getLastGame(plid, now, file_str);
        switch (status) {
        case Game::Status::ACT:
            reply->status = ReplyShowTrialsPacket::ACT;
            break;
        case Game::Status::FIN:
            reply->status = ReplyShowTrialsPacket::FIN;
            break;
        default:
            break;
        }

        reply->fname = "STATE_" + plid + ".txt";
        reply->fsize = file_str.size();
        reply->fdata = file_str + '\n';

        std::stringstream ss;
        ss << "[Player " << request.playerID << "] > Requested to show last game. (" << reply->fsize << " Bytes)\n";
        state.logger.log(Logger::Severity::INFO, ss.str(), true);
    } catch (const std::exception& e) {
        reply->status = ReplyShowTrialsPacket::NOK;
        state.logger.log(Logger::Severity::WARN, e.what(), true);
    }

    replyPacket = std::move(reply);
}

void showScoreboardHandler(const int& fd, Server& state, std::unique_ptr<TcpPacket>& replyPacket) {
    ShowScoreboardPacket request;
    auto reply = std::make_unique<ReplyShowScoreboardPacket>();
    reply->status = ReplyShowScoreboardPacket::EMPTY;

    try {
        request.read(fd);

        std::string file_str = state.store.getScoreboard();
        
        reply->fname = "TOPSCORES.txt";
        reply->fsize = file_str.size();
        reply->fdata = file_str + '\n';
        reply->status = ReplyShowScoreboardPacket::OK;

        std::stringstream ss;
        ss << "Sending scoreboard... (" << reply->fsize << " Bytes)\n";
        state.logger.log(Logger::Severity::INFO, ss.str(), true);
    } catch (const EmptyScoreboardException& e) {
        reply->status = ReplyShowScoreboardPacket::EMPTY;
        state.logger.log(Logger::Severity::INFO, e.what(), true);
    } catch (const std::exception& e) {
        reply->status = ReplyShowScoreboardPacket::EMPTY;
        state.logger.log(Logger::Severity::WARN, e.what(), true);
    }

    replyPacket = std::move(reply);
}