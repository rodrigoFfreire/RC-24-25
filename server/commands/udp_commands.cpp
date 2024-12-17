#include <chrono>

#include "udp_commands.hpp"
#include "../exceptions/GameExceptions.hpp"


/// @brief Start new game handler. Creates a new game if no active game was found
/// @param packetStream Stream that contains the received packet
/// @param store GameStore object responsible for managing the database
/// @param logger Logger object for logging useful information
/// @param replyPacket Will store the reply packet to be sent later
void startNewGameHandler(std::stringstream& packetStream, GameStore& store, Logger& logger, std::unique_ptr<UdpPacket>& replyPacket) {
    StartNewGamePacket request;
    auto reply = std::make_unique<ReplyStartGamePacket>();
    reply->status = ReplyStartGamePacket::OK;

    try {
        std::time_t now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
        request.decode(packetStream);

        std::string key = store.createGame(request.playerID, now, request.time, nullptr);

        std::stringstream log_ss;
        log_ss << "[Player " << request.playerID << "] > New game (max " << request.time << "s); " << key;
        logger.log(Logger::Severity::INFO, log_ss.str(), true);
    } catch (const OngoingGameException& e) {
        reply->status = ReplyStartGamePacket::NOK;  // Player alrady has an ongoing game
        logger.log(Logger::Severity::WARN, e.what(), true);
    } catch (const std::exception& e) {
        reply->status = ReplyStartGamePacket::ERR;  // Some other error (i.e: syntax)
        logger.log(Logger::Severity::WARN, e.what(), true);
    }

    replyPacket = std::move(reply);
}

/// @brief Try handler. Sends an attempt to an ongoing game
/// @param packetStream Stream that contains the received packet
/// @param store GameStore object responsible for managing the database
/// @param logger Logger object for logging useful information
/// @param replyPacket Will store the reply packet to be sent later
void tryHandler(std::stringstream &packetStream, GameStore& store, Logger& logger, std::unique_ptr<UdpPacket>& replyPacket) {
    TryPacket request;
    auto reply = std::make_unique<ReplyTryPacket>();
    reply->status = ReplyTryPacket::OK;

    std::string real_key;

    try {
        time_t now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
        request.decode(packetStream);

        reply->trial = store.attempt(request.playerID, now, request.key, request.trial, reply->blacks, reply->whites, real_key);

        std::stringstream ss;
        ss << "[Player " << request.playerID << "] > Try " << request.key << ' ';
        ss << "- nB = " << reply->blacks << ", nW = " << reply->whites;
        ss << (reply->blacks == SECRET_KEY_LEN ? "; GUESSED!" : "; NOT GUESSED");
        logger.log(Logger::Severity::INFO, ss.str(), true);
    } catch (const DuplicateTrialException& e) {
        reply->status = ReplyTryPacket::DUP;    // Duplicate key
        logger.log(Logger::Severity::WARN, e.what(), true);
    } catch (const InvalidTrialException& e) {
        reply->status = ReplyTryPacket::INV;    // Invalid Trial number
        logger.log(Logger::Severity::WARN, e.what(), true);
    } catch (const UncontextualizedGameException& e) {
        reply->status = ReplyTryPacket::NOK;    // No ongoing game
        logger.log(Logger::Severity::WARN, e.what(), true);
    } catch (const ExceededMaxTrialsException& e) {
        reply->status = ReplyTryPacket::ENT;    // Max trials exceeded
        reply->key = real_key;
        logger.log(Logger::Severity::WARN, e.what(), true);
    } catch (const TimedoutGameException& e) {
        reply->status = ReplyTryPacket::ETM;    // Max time exceeded
        reply->key = real_key;
        logger.log(Logger::Severity::WARN, e.what(), true);
    } catch (const std::exception& e) {
        reply->status = ReplyTryPacket::ERR;    // Some other error (i.e: invalid syntax)
        logger.log(Logger::Severity::WARN, e.what(), true);
    }

    replyPacket = std::move(reply);
}

/// @brief Quit handler. Quit an ongoing game
/// @param packetStream Stream that contains the received packet
/// @param store GameStore object responsible for managing the database
/// @param logger Logger object for logging useful information
/// @param replyPacket Will store the reply packet to be sent later
void quitHandler(std::stringstream &packetStream, GameStore& store, Logger& logger, std::unique_ptr<UdpPacket>& replyPacket) {
    QuitPacket request;
    auto reply = std::make_unique<ReplyQuitPacket>();
    reply->status = ReplyQuitPacket::OK;

    try {
        std::time_t now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
        request.decode(packetStream);

        reply->key = store.quitGame(request.playerID, now);

        std::stringstream ss;
        ss << "[Player " << request.playerID << "] > Quit game; Secret key: " << reply->key;
        logger.log(Logger::Severity::INFO, ss.str(), true);
    } catch (const UncontextualizedGameException& e) {
        reply->status = ReplyQuitPacket::NOK;   // No ongoing game found
        logger.log(Logger::Severity::WARN, e.what(), true);
    } catch (const std::exception& e) {
        reply->status = ReplyQuitPacket::ERR;   // Some other error (i.e: syntax error)
        logger.log(Logger::Severity::WARN, e.what(), true);
    }

    replyPacket = std::move(reply);
}

/// @brief Debug game handler. Creates a new debug game if possible
/// @param packetStream Stream that contains the received packet
/// @param store GameStore object responsible for managing the database
/// @param logger Logger object for logging useful information
/// @param replyPacket Will store the reply packet to be sent later
void debugGameHandler(std::stringstream &packetStream, GameStore& store, Logger& logger, std::unique_ptr<UdpPacket>& replyPacket) {
    DebugPacket request;
    auto reply = std::make_unique<ReplyDebugPacket>();
    reply->status = ReplyDebugPacket::OK;

    try {
        std::time_t now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
        request.decode(packetStream);

        store.createGame(request.playerID, now, request.time, &request.key);

        std::stringstream ss;
        ss << "[Player " << request.playerID << "] > Debug game (max " << request.time << "s); " << request.key;
        logger.log(Logger::Severity::INFO, ss.str(), true);
    } catch (const OngoingGameException& e) {
        reply->status = ReplyDebugPacket::NOK;      // Player already has an ongoing game
        logger.log(Logger::Severity::WARN, e.what(), true);
    } catch (const std::exception& e) {
        reply->status = ReplyDebugPacket::ERR;      // Some other error (i.e: syntax error)
        logger.log(Logger::Severity::WARN, e.what(), true);
    }

    replyPacket = std::move(reply);
}
