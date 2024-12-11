#ifndef SERVER_GAME_STORE_HPP
#define SERVER_GAME_STORE_HPP

#include <string>
#include <filesystem>
#include <fstream>
#include <array>
#include <unordered_map>
#include <sstream>
#include <chrono>
#include <random>
#include <ctime>
#include "../common/constants.hpp"
#include "exceptions/ServerExceptions.hpp"
#include "exceptions/GameExceptions.hpp"

namespace fs = std::filesystem;

void formatDateTime(std::stringstream& ss, std::time_t &tstamp, char sep, bool pretty);

enum GameMode { PLAY, DEBUG };

class Attempt {
public:
    std::string att_key;
    uint blacks;
    uint whites;
    uint time;

    Attempt(std::string& att);
    static std::string create(std::string& att, uint blacks, uint whites, std::time_t time);
};

class ActiveGame {
public:
    std::string plid;
    std::string key;
    std::string date_start;
    std::string time_start;
    GameMode mode;
    uint playTime;
    int tstamp_start;

    void parseHeader(std::ifstream& file);
    uint parseAttempts(std::ifstream& file, std::string& att, std::string& last_att, bool& dup);
    static std::string create(std::string& plid, uint playTime, GameMode mode, std::time_t& cmd_tstamp, std::string& key);
};

class GameStore {
private:
    std::string generateKey();

public:
    enum Endings { WIN, LOST, QUIT, TIMEOUT };

    GameStore(std::string& dir);

    int updateGameTime(std::string& plid, std::time_t& cmd_tstamp, std::string* revealed_key);
    uint attempt(std::string& plid, std::string& key, uint trial, uint& blacks, uint& whites, std::time_t& cmd_tstamp, std::string& real_key);
    std::string quitGame(std::string& plid, std::time_t& cmd_tstamp);
    std::string createGame(std::string& plid, uint playTime, std::time_t& cmd_tstamp, std::string* key);
    void endGame(std::string& plid, Endings reason, std::time_t& tstamp, std::ofstream& file, int play_time);

private:
    fs::path storeDir;

    void calculateAttempt(std::string& key, std::string& att, uint& whites, uint& blacks);
    void saveGameScore(std::string& plid, std::time_t& win_tstamp, uint used_atts, uint used_time);
    char endingToStr(Endings ending);
};

#endif