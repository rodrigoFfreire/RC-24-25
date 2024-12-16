#include "udp_commands.hpp"

std::string parsePlayerID(std::stringstream& command_stream) {
    int plid_num = -1;

    command_stream >> plid_num;
    if (command_stream.fail()) {
        throw InvalidPlayerIDException();
    }
    if (plid_num < 0 || plid_num > PLID_MAX) {
        throw InvalidPlayerIDException();
    }

    std::ostringstream formatted_plid;
    formatted_plid << std::setw(PLID_LEN) << std::setfill('0') << plid_num;
    return formatted_plid.str();
}

unsigned short parsePlayTime(std::stringstream& command_stream) {
    short play_time = -1;

    command_stream >> play_time;
    if (command_stream.fail()) {
        throw InvalidPlayTimeException();
    }
    if (play_time <= 0 || play_time > PLAY_TIME_MAX) {
        throw InvalidPlayTimeException();
    }

    return static_cast<unsigned short>(play_time);
}

std::string parseKey(std::stringstream& command_stream) {
    std::string key(SECRET_KEY_LEN, '\0');
    std::string valid_colors = VALID_COLORS;
    
    for (size_t i = 0; i < SECRET_KEY_LEN; i++) {
        char c;
        command_stream >> c;
        c = std::toupper(c);
        if (valid_colors.find(c) == std::string::npos || command_stream.eof()) {
            throw InvalidKeyException();
        }
        key[i] = c;
    }

    return key;
}

void startNewGameHandler(GameState& state, UdpSocket& socket, std::stringstream& command_stream) {
    StartNewGamePacket request;
    ReplyStartGamePacket reply;
    std::stringstream responseStream;

    try {
        request.playerID = parsePlayerID(command_stream);
        request.time = parsePlayTime(command_stream);

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

void tryHandler(GameState& state, UdpSocket& socket, std::stringstream& command_stream) {
    TryPacket request;
    ReplyTryPacket reply;
    std::stringstream responseStream;

    try {
        if (!state.isGame()) {
            throw UncontextualizedException();
        }
        
        request.key = parseKey(command_stream);
        request.playerID = state.getPlid();
        request.trial = state.getTrial();

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

void debugGameHandler(GameState& state, UdpSocket& socket, std::stringstream& command_stream) {
    DebugPacket request;
    ReplyDebugPacket reply;
    std::stringstream responseStream;

    try {
        request.playerID = parsePlayerID(command_stream);
        request.time = parsePlayTime(command_stream);
        request.key = parseKey(command_stream);

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