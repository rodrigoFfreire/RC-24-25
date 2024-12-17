#include <chrono>

#include "tcp_commands.hpp"
#include "../exceptions/GameExceptions.hpp"


/// @brief Show trials handler. Provides the information about the last game (active or finished)
/// @param fd TCP connection descriptor
/// @param store GameStore object responsible for managing the database
/// @param logger Logger object for logging useful information
/// @param replyPacket Will store the reply packet to be sent later
void showTrialsHandler(const int fd, GameStore& store, Logger& logger, std::unique_ptr<TcpPacket>& replyPacket) {
    ShowTrialsPacket request;
    auto reply = std::make_unique<ReplyShowTrialsPacket>();
    reply->status = ReplyShowTrialsPacket::NOK;

    try {
        time_t now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
        request.read(fd);

        std::string file_str;

        Game::Status status = store.getLastGame(request.playerID, now, file_str);
        switch (status) {
        case Game::Status::ACT:
            reply->status = ReplyShowTrialsPacket::ACT;     // Fetched game is still active
            break;
        case Game::Status::FIN:
            reply->status = ReplyShowTrialsPacket::FIN;     // Fetched game is finished
            break;
        default:
            break;
        }

        reply->fname = "STATE_" + request.playerID + ".txt";
        reply->fsize = file_str.size();
        reply->fdata = file_str + '\n';

        std::stringstream ss;
        ss << "[Player " << request.playerID << "] > Requested to show last game. (" << reply->fsize << " Bytes)";
        logger.log(Logger::Severity::INFO, ss.str(), true);
    } catch (const std::exception& e) {
        reply->status = ReplyShowTrialsPacket::NOK;     // Some other error (i.e: syntax error)
        logger.log(Logger::Severity::WARN, e.what(), true);
    }

    replyPacket = std::move(reply);
}


/// @brief Show scoreboard handler. Provides a scoreboard of the best games
/// @param fd TCP connection descriptor
/// @param store GameStore object responsible for managing the database
/// @param logger Logger object for logging useful information
/// @param replyPacket Will store the reply packet to be sent later
void showScoreboardHandler(const int fd, GameStore& store, Logger& logger, std::unique_ptr<TcpPacket>& replyPacket) {
    ShowScoreboardPacket request;
    auto reply = std::make_unique<ReplyShowScoreboardPacket>();
    reply->status = ReplyShowScoreboardPacket::EMPTY;

    try {
        request.read(fd);

        std::string file_str = store.getScoreboard();
        
        reply->fname = "TOPSCORES.txt";
        reply->fsize = file_str.size();
        reply->fdata = file_str + '\n';
        reply->status = ReplyShowScoreboardPacket::OK;

        std::stringstream ss;
        ss << "Sending scoreboard... (" << reply->fsize << " Bytes)";
        logger.log(Logger::Severity::INFO, ss.str(), true);
    } catch (const EmptyScoreboardException& e) {
        reply->status = ReplyShowScoreboardPacket::EMPTY;   // Scoreboard is empty
        logger.log(Logger::Severity::WARN, e.what(), true);
    } catch (const std::exception& e) {
        reply->status = ReplyShowScoreboardPacket::EMPTY;   // If some other error occurs make the scoreboard empty
        logger.log(Logger::Severity::WARN, e.what(), true);
    }

    replyPacket = std::move(reply);
}