#include "udp_commands.hpp"

#include "../../common/protocol/UDP/udp.hpp"

void startNewGameHandler(std::stringstream& packetStream, Server& state, std::unique_ptr<UdpPacket>& replyPacket) {
    StartNewGamePacket request;
    auto reply = std::make_unique<ReplyStartGamePacket>();
    reply->status = ReplyStartGamePacket::OK;

    try {
        std::time_t now = state.getCommandTime();
        request.decode(packetStream);

        std::ostringstream oss;
        oss << std::setw(PLAYER_ID_LEN) << std::setfill('0') << request.playerID;
        std::string plid = oss.str();

        std::string key = state.store.createGame(plid, request.time, now, NULL);

        std::stringstream ss;
        ss << "[Player " << plid << "] > New game (max " << request.time << "s); " << key;
        state.logger.log(Logger::Severity::INFO, ss.str(), true);
    } catch (const OngoingGameException& e) {
        reply->status = ReplyStartGamePacket::NOK;
        state.logger.log(Logger::Severity::WARN, e.what(), true);
    } catch (const std::exception& e) {
        reply->status = ReplyStartGamePacket::ERR;
        state.logger.log(Logger::Severity::WARN, e.what(), true);
    }

    replyPacket = std::move(reply);
}

void tryHandler(std::stringstream &packetStream, Server &state, std::unique_ptr<UdpPacket>& replyPacket) {
    TryPacket request;
    auto reply = std::make_unique<ReplyTryPacket>();
    reply->status = ReplyTryPacket::OK;

    std::string real_key;

    try {
        time_t now = state.getCommandTime();
        request.decode(packetStream);

        std::ostringstream oss;
        oss << std::setw(PLAYER_ID_LEN) << std::setfill('0') << request.playerID;
        std::string plid = oss.str();

        reply->trial = state.store.attempt(plid, request.key, request.trial, reply->blacks, reply->whites, now, real_key);

        std::stringstream ss;
        ss << "[Player " << request.playerID << "] > Try " << request.key << ' ';
        ss << "- nB = " << reply->blacks << ", nW = " << reply->whites;
        state.logger.log(Logger::Severity::INFO, ss.str(), true);
    } catch (const DuplicateTrialException& e) {
        reply->status = ReplyTryPacket::DUP;
        state.logger.log(Logger::Severity::WARN, e.what(), true);
    } catch (const InvalidTrialException& e) {
        reply->status = ReplyTryPacket::INV;
        state.logger.log(Logger::Severity::WARN, e.what(), true);
    } catch (const UncontextualizedGameException& e) {
        reply->status = ReplyTryPacket::NOK;
        state.logger.log(Logger::Severity::WARN, e.what(), true);
    } catch (const ExceededMaxTrialsException& e) {
        reply->status = ReplyTryPacket::ENT;
        reply->key = real_key;
        state.logger.log(Logger::Severity::WARN, e.what(), true);
    } catch (const TimedoutGameException& e) {
        reply->status = ReplyTryPacket::ETM;
        reply->key = real_key;
        state.logger.log(Logger::Severity::WARN, e.what(), true);
    } catch (const std::exception& e) {
        reply->status = ReplyTryPacket::ERR;
        state.logger.log(Logger::Severity::WARN, e.what(), true);
    }

    replyPacket = std::move(reply);
}

void quitHandler(std::stringstream &packetStream, Server &state, std::unique_ptr<UdpPacket>& replyPacket) {
    QuitPacket request;
    auto reply = std::make_unique<ReplyQuitPacket>();
    reply->status = ReplyQuitPacket::OK;

    try {
        std::time_t now = state.getCommandTime();
        request.decode(packetStream);

        std::ostringstream oss;
        oss << std::setw(PLAYER_ID_LEN) << std::setfill('0') << request.playerID;
        std::string plid = oss.str();

        reply->key = state.store.quitGame(plid, now);

        std::stringstream ss;
        ss << "[Player " << plid << "] > Quit game; Secret key: " << reply->key;
        state.logger.log(Logger::Severity::INFO, ss.str(), true);
    } catch (const UncontextualizedGameException& e) {
        reply->status = ReplyQuitPacket::NOK;
        state.logger.log(Logger::Severity::WARN, e.what(), true);
    } catch (const std::exception& e) {
        reply->status = ReplyQuitPacket::ERR;
        state.logger.log(Logger::Severity::WARN, e.what(), true);
    }

    replyPacket = std::move(reply);
}

void debugGameHandler(std::stringstream &packetStream, Server &state, std::unique_ptr<UdpPacket>& replyPacket) {
    DebugPacket request;
    auto reply = std::make_unique<ReplyDebugPacket>();
    reply->status = ReplyDebugPacket::OK;

    try {
        std::time_t now = state.getCommandTime();
        request.decode(packetStream);

        std::ostringstream oss;
        oss << std::setw(PLAYER_ID_LEN) << std::setfill('0') << request.playerID;
        std::string plid = oss.str();
        std::string key(request.key);

        (void)state.store.createGame(plid, request.time, now, &key);

        std::stringstream ss;
        ss << "[Player " << plid << "] > Debug game (max " << request.time << "s); " << key;
        state.logger.log(Logger::Severity::INFO, ss.str(), true);
    } catch (const OngoingGameException& e) {
        reply->status = ReplyDebugPacket::NOK;
        state.logger.log(Logger::Severity::WARN, e.what(), true);
    } catch (const std::exception& e) {
        reply->status = ReplyDebugPacket::ERR;
        state.logger.log(Logger::Severity::WARN, e.what(), true);
    }

    replyPacket = std::move(reply);
}
