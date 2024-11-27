#include "udp_commands.hpp"

#include "../../common/protocol/UDP/udp.hpp"

void startNewGameHandler(std::stringstream& packetStream, Server& state, std::unique_ptr<UdpPacket>& replyPacket) {
    StartNewGamePacket request;
    auto reply = std::make_unique<ReplyStartGamePacket>();
    reply->status = ReplyStartGamePacket::ERR;

    try {
        request.decode(packetStream);
        
        // Create game
        std::string key = "R R R R R";
        reply->status = ReplyStartGamePacket::OK;

        std::stringstream ss;
        ss << "[Player " << request.playerID << "] > New game (max " << request.time << "s); " << key;
        state.logger.log(Logger::Severity::INFO, ss.str(), true);

    } catch (const InvalidPacketException& e) {
        reply->status = ReplyStartGamePacket::ERR;
        state.logger.log(Logger::Severity::WARN, e.what(), true);
    }

    replyPacket = std::move(reply);
}

void tryHandler(std::stringstream &packetStream, Server &state, std::unique_ptr<UdpPacket>& replyPacket) {
    TryPacket request;
    auto reply = std::make_unique<ReplyTryPacket>();
    reply->status = ReplyTryPacket::ERR;

    try {
        request.decode(packetStream);

        // Attempt
        std::string guessed = "not guessed";
        reply->trial = request.trial;
        reply->whites = 69;
        reply->blacks = 69;
        reply->status = ReplyTryPacket::OK;

        std::stringstream ss;
        ss << "[Player " << request.playerID << "] > Try ";
        for (int i = 0; i < SECRET_KEY_LEN; ++i) {
            ss << request.key[i] << " ";
        }
        ss << "- nB = " << reply->blacks << ", nW = " << reply->whites << "; " << guessed;
        state.logger.log(Logger::Severity::INFO, ss.str(), true);
        
    } catch (const InvalidPacketException& e) {
        reply->status = ReplyTryPacket::ERR;
        state.logger.log(Logger::Severity::WARN, e.what(), true);
    }

    replyPacket = std::move(reply);
}

void quitHandler(std::stringstream &packetStream, Server &state, std::unique_ptr<UdpPacket>& replyPacket) {
    QuitPacket request;
    auto reply = std::make_unique<ReplyQuitPacket>();
    reply->status = ReplyQuitPacket::ERR;

    try {
        request.decode(packetStream);

        // Quit
        reply->status = ReplyQuitPacket::OK;
        char key[SECRET_KEY_LEN] = {'R', 'G', 'B', 'G'};

        std::stringstream ss;
        ss << "[Player " << request.playerID << "] > Quit game. Key:";
        for (int i = 0; i < SECRET_KEY_LEN; i++) {
            ss << ' ' << key[i];
        }

        state.logger.log(Logger::Severity::INFO, ss.str(), true);
    } catch (const InvalidPacketException& e) {
        reply->status = ReplyQuitPacket::ERR;
        state.logger.log(Logger::Severity::WARN, e.what(), true);
    }

    replyPacket = std::move(reply);
}

void debugGameHandler(std::stringstream &packetStream, Server &state, std::unique_ptr<UdpPacket>& replyPacket) {
    DebugPacket request;
    auto reply = std::make_unique<ReplyDebugPacket>();
    reply->status = ReplyDebugPacket::ERR;

    try {
        request.decode(packetStream);

        // DBG
        reply->status = ReplyDebugPacket::OK;
        std::stringstream ss;
        ss << "[Player " << request.playerID << "] > Debug game. Key:";
        for (int i = 0; i < SECRET_KEY_LEN; i++) {
            ss << ' ' << request.key[i];
        }
        state.logger.log(Logger::Severity::INFO, ss.str(), true);
    } catch (const InvalidPacketException& e) {
        reply->status = ReplyDebugPacket::ERR;
        state.logger.log(Logger::Severity::WARN, e.what(), true);
    }

    replyPacket = std::move(reply);
}
