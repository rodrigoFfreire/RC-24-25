#include "udp_commands.hpp"

void buildStartGamePacket(std::stringstream& command_stream, StartNewGamePacket& packet) {
    std::string playerID;
    int playTime;
    
    command_stream >> playerID;
    command_stream >> playTime;

    long plid;
    try {
        plid = std::stoul(playerID);
        if (plid < 0 || plid > PLID_MAX) {
            throw InvalidPlayerIDException();
        }
    } catch (const std::exception& e) {
        throw InvalidPlayerIDException();
    }

    if (playTime < 1 || playTime > PLAY_TIME_MAX) {
        throw InvalidPlayTimeException();
    }

    packet.playerID = static_cast<unsigned int>(plid);
    packet.time = playTime;
}

void startNewGameHandler(GameState& state, UdpSocket& socket, std::stringstream& command_stream) {
    StartNewGamePacket request;
    ReplyStartGamePacket reply;
    std::stringstream responseStream;

    try {
        buildStartGamePacket(command_stream, request);
        socket.sendPacket(&request);
        socket.receivePacket(responseStream);

        responseStream >> std::noskipws;
        reply.decode(responseStream);

        switch (reply.status) {
        case ReplyStartGamePacket::OK:
            std::cout << "Game started!" << std::endl;
            state.startGame(request.playerID, nullptr, false);
            break;
        case ReplyStartGamePacket::NOK:
            throw PendingGameException();
            break;
        case ReplyStartGamePacket::ERR:        
        default:
            throw BadCommandException();
        }
    } catch (const CommonException& e) {
        std::cout << e.what() << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "ERROR: " << e.what() << std::endl;
    }
}

void buildTryPacket(GameState& state, std::stringstream& command_stream, TryPacket& packet) {
    std::string valid_colors = VALID_COLORS;
    packet.key.resize(SECRET_KEY_LEN, '\0');
    
    for (size_t i = 0; i < SECRET_KEY_LEN; i++) {
        char c;
        command_stream >> c;
        c = std::toupper(c);
        if (valid_colors.find(c) == std::string::npos || command_stream.eof()) {
            throw InvalidKeyException();
        }
        packet.key[i] = c;
    }
    packet.playerID = state.getPlid();
    packet.trial = state.getTrial();
}

void tryHandler(GameState& state, UdpSocket& socket, std::stringstream& command_stream) {
    TryPacket request;
    ReplyTryPacket reply;
    std::stringstream responseStream;

    try {
        if (!state.isGame()) {
            throw UncontextualizedException();
        }
        
        buildTryPacket(state, command_stream, request);
        socket.sendPacket(&request);
        socket.receivePacket(responseStream);

        responseStream >> std::noskipws;
        reply.decode(responseStream);

        switch (reply.status) {
        case ReplyTryPacket::OK:
            if (reply.trial < request.trial)
                break;
            state.saveAttempt(request.key, reply.blacks, reply.whites);
            if (reply.blacks == SECRET_KEY_LEN) {
                state.endGame(request.key, GameState::Events::WON);
                break;
            }
            state.newAttempt();
            state.printState();
            break;
        case ReplyTryPacket::DUP:
            throw DupAttemptException();
        case ReplyTryPacket::INV:
            throw InvalidAttemptException();
        case ReplyTryPacket::NOK:
            throw UncontextualizedException();
        case ReplyTryPacket::ENT:
            state.saveAttempt(request.key, 0, 0);
            state.endGame(reply.key, GameState::Events::LOST_MAXTRIALS);
            break;
        case ReplyTryPacket::ETM:
            state.saveAttempt(request.key, 0, 0);
            state.endGame(reply.key, GameState::Events::LOST_TIME);
            break;
        case ReplyTryPacket::ERR:
        default:
            throw BadCommandException();
        }
    } catch (const CommonException& e) {
        std::cout << e.what() << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "ERROR: " << e.what() << std::endl;
    }
}

void quitHandler(GameState& state, UdpSocket& socket, std::stringstream& command_stream) {
    (void)command_stream;
    QuitPacket request;
    ReplyQuitPacket reply;
    std::stringstream responseStream;

    try {
        request.playerID = state.getPlid();
        socket.sendPacket(&request);
        socket.receivePacket(responseStream);
        
        responseStream >> std::noskipws;
        reply.decode(responseStream);

        switch (reply.status) {
        case ReplyQuitPacket::OK:
            state.endGame(reply.key, GameState::Events::QUIT);
            break;
        case ReplyQuitPacket::NOK:
            throw UncontextualizedException();
        case ReplyQuitPacket::ERR:
        default:
            throw BadCommandException();
        }
    } catch (const CommonException& e) {
        std::cout << e.what() << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "ERROR: " << e.what() << std::endl;
    }
}

void buildDebugGamePacket(std::stringstream& command_stream, DebugPacket& packet) {
    std::string playerID;
    int playTime;
    
    command_stream >> playerID;
    command_stream >> playTime;

    long plid;
    try {
        plid = std::stoul(playerID);
        if (plid < 0 || plid > PLID_MAX) {
            throw InvalidPlayerIDException();
        }
    } catch (const std::exception& e) {
        throw InvalidPlayerIDException();
    }

    if (playTime < 1 || playTime > PLAY_TIME_MAX) {
        throw InvalidPlayTimeException();
    }

    std::string valid_colors = VALID_COLORS;
    for (size_t i = 0; i < SECRET_KEY_LEN; i++) {
        char c;
        command_stream >> c;
        c = std::toupper(c);
        if (valid_colors.find(c) == std::string::npos || command_stream.eof()) {
            throw InvalidKeyException();
        }
        packet.key[i] = c;
    }

    packet.playerID = static_cast<unsigned int>(plid);
    packet.time = playTime;
}

void debugGameHandler(GameState& state, UdpSocket& socket, std::stringstream& command_stream) {
    DebugPacket request;
    ReplyDebugPacket reply;
    std::stringstream responseStream;

    try {
        buildDebugGamePacket(command_stream, request);
        socket.sendPacket(&request);
        socket.receivePacket(responseStream);

        responseStream >> std::noskipws;
        reply.decode(responseStream);
        switch (reply.status) {
        case ReplyDebugPacket::OK:
            state.startGame(request.playerID, &request.key, true);
            break;
        case ReplyDebugPacket::NOK:
            throw PendingGameException();
        case ReplyDebugPacket::ERR:
        default:
            break;
        }
    } catch (const CommonException& e) {
        std::cout << e.what() << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "ERROR: " << e.what() << std::endl;
    }
}