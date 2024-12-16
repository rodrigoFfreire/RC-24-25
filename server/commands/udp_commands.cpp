#include <chrono>

#include "udp_commands.hpp"
#include "../exceptions/GameExceptions.hpp"


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
        reply->status = ReplyStartGamePacket::NOK;
        logger.log(Logger::Severity::WARN, e.what(), true);
    } catch (const std::exception& e) {
        reply->status = ReplyStartGamePacket::ERR;
        logger.log(Logger::Severity::WARN, e.what(), true);
    }

    replyPacket = std::move(reply);
}

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
        reply->status = ReplyTryPacket::DUP;
        logger.log(Logger::Severity::WARN, e.what(), true);
    } catch (const InvalidTrialException& e) {
        reply->status = ReplyTryPacket::INV;
        logger.log(Logger::Severity::WARN, e.what(), true);
    } catch (const UncontextualizedGameException& e) {
        reply->status = ReplyTryPacket::NOK;
        logger.log(Logger::Severity::WARN, e.what(), true);
    } catch (const ExceededMaxTrialsException& e) {
        reply->status = ReplyTryPacket::ENT;
        reply->key = real_key;
        logger.log(Logger::Severity::WARN, e.what(), true);
    } catch (const TimedoutGameException& e) {
        reply->status = ReplyTryPacket::ETM;
        reply->key = real_key;
        logger.log(Logger::Severity::WARN, e.what(), true);
    } catch (const std::exception& e) {
        reply->status = ReplyTryPacket::ERR;
        logger.log(Logger::Severity::WARN, e.what(), true);
    }

    replyPacket = std::move(reply);
}

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
        reply->status = ReplyQuitPacket::NOK;
        logger.log(Logger::Severity::WARN, e.what(), true);
    } catch (const std::exception& e) {
        reply->status = ReplyQuitPacket::ERR;
        logger.log(Logger::Severity::WARN, e.what(), true);
    }

    replyPacket = std::move(reply);
}

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
        reply->status = ReplyDebugPacket::NOK;
        logger.log(Logger::Severity::WARN, e.what(), true);
    } catch (const std::exception& e) {
        reply->status = ReplyDebugPacket::ERR;
        logger.log(Logger::Severity::WARN, e.what(), true);
    }

    replyPacket = std::move(reply);
}
