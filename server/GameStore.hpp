#ifndef SERVER_GAME_STORE_HPP
#define SERVER_GAME_STORE_HPP

#include <string>
#include <filesystem>
#include <fstream>
#include <array>
#include <algorithm>
#include <vector>
#include <unordered_map>
#include <sstream>
#include <random>
#include "../common/constants.hpp"
#include "exceptions/ServerExceptions.hpp"
#include "exceptions/GameExceptions.hpp"
#include "../common/utils.hpp"

namespace fs = std::filesystem;

enum GameMode { PLAY, DEBUG };
enum Endings { WIN, LOST, QUIT, TIMEOUT };

std::string gameModeToRepr(GameMode mode);
GameMode charToGameMode(const char c);

std::string endingToRepr(Endings ending);
Endings charToEnding(const char c);

class Attempt {
public:
    std::string att_key;
    uint blacks;
    uint whites;
    uint time;

    Attempt(std::string& att);
    static std::string create(std::string& att, uint blacks, uint whites, time_t time);
};

class Game {
public:
    enum Status { FIN, ACT };

    std::string plid;
    std::string key;
    std::string date_start;
    std::string date_end;
    std::string time_start;
    std::string time_end;

    std::vector<Attempt> attempts;
    GameMode mode;
    Endings ending;
    Status status;
    uint playTime;
    uint usedTime;
    int tstamp_start;

    void parseGame(std::ifstream& file);
    void parseHeader(std::ifstream& file);
    uint parseAttempts(std::ifstream& file, std::string& att, std::string& last_att, bool& dup);
    static std::string create(std::string& plid, uint playTime, GameMode mode, time_t& cmd_tstamp, std::string& key);
};

class GameStore {
private:
    std::string generateKey();

public:
    GameStore(std::string& dir);

    std::string createGame(std::string& plid, uint playTime, time_t& cmd_tstamp, std::string* key);
    uint attempt(std::string& plid, std::string& key, uint trial, uint& blacks, uint& whites, time_t& cmd_tstamp, std::string& real_key);
    std::string quitGame(std::string& plid, time_t& cmd_tstamp);
    Game::Status getLastGame(std::string& plid, time_t& cmd_tstamp, std::string& output);
    std::string getScoreboard(time_t& cmd_tstamp);

private:
    fs::path storeDir;

    int updateGameTime(std::string& plid, time_t& cmd_tstamp, std::string* revealed_key);
    void calculateAttempt(std::string& key, std::string& att, uint& whites, uint& blacks);
    void saveGameScore(std::string &plid, std::string& key, GameMode mode, time_t &win_tstamp, int used_atts, int used_time);
    void endGame(std::string& plid, Endings reason, time_t& tstamp, std::ofstream& file, int play_time);
    std::string findLastFinishedGame(std::string& plid);
};

#endif