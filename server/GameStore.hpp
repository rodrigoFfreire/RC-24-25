#ifndef SERVER_GAME_STORE_HPP
#define SERVER_GAME_STORE_HPP

#include <string>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <chrono>
#include <random>
#include <ctime>
#include "../common/constants.hpp"
#include "exceptions/ServerExceptions.hpp"
#include "exceptions/GameExceptions.hpp"

#define GAME_PLAYTIME_OFFSET 14
#define GAME_TSTAMP_OFFSET 2

namespace fs = std::filesystem;

class GameStore {
public:
    enum Events { GAME_FOUND, GAME_TIMEDOUT, GAME_NOT };
    enum Endings { WIN, LOST, QUIT, TIMEOUT };
    enum GameMode { PLAY, DEBUG };

    GameStore(std::string& dir);

    int updateGameTime(std::string& plid, std::time_t& cmd_tstamp);
    std::string createGame(std::string& plid, unsigned short playTime, GameMode gamemode, std::time_t& cmd_tstamp);
    void endGame(std::string& plid, Endings reason, std::time_t& tstamp, std::ofstream& file, int play_time);

private:
    fs::path storeDir;
    
    std::string generateKey();
    void formatDateTime(std::stringstream& ss, std::time_t& tstamp, char sep, bool pretty);
    char endingToStr(Endings ending);
};

#endif