#ifndef SERVER_GAME_STORE_HPP
#define SERVER_GAME_STORE_HPP

#include <filesystem>
#include <vector>

enum GameMode { PLAY, DEBUG };
enum Endings { WIN, LOST, QUIT, TIMEOUT };

std::string gameModeToRepr(const GameMode mode);
GameMode charToGameMode(const char c);

std::string endingToRepr(const Endings ending);
Endings charToEnding(const char c);

class LeaderboardEntry {
 public:
  int score;
  std::string plid;
  std::string key;
  uint used_atts;
  GameMode mode;

  LeaderboardEntry(std::ifstream& file);
};

class Attempt {
 public:
  std::string att_key;
  uint blacks;
  uint whites;
  uint time;

  Attempt(const std::string& att);
  static std::string create(const std::string& key, const uint blacks, const uint whites,
                            const time_t time);
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
  uint parseAttempts(std::ifstream& file, const std::string& key, std::string& last_att,
                     bool& dup);
  static std::string create(const std::string& plid, const uint playTime,
                            const GameMode mode, const time_t& cmd_tstamp,
                            const std::string& key);
};

class GameStore {
 private:
  std::filesystem::path storeDir;

  int checkTimedoutGame(const std::string& plid, const time_t& cmd_tstamp,
                        std::string* revealed_key);
  void calculateAttempt(const std::string& key, const std::string& att, uint& whites,
                        uint& blacks);
  void saveGameScore(const std::string& plid, const std::string& key, const GameMode mode,
                     const time_t& win_tstamp, const int used_atts, const int used_time);
  void endGame(const std::string& plid, const Endings reason, const time_t& tstamp,
               std::ofstream& file, const int play_time);
  std::string generateKey();
  std::string findLastFinishedGame(const std::string& plid);

 public:
  GameStore(const std::string& dir);

  std::string createGame(const std::string& plid, const time_t& cmd_tstamp,
                         const uint playTime, std::string* key);
  uint attempt(const std::string& plid, const time_t& cmd_tstamp, const std::string& att,
               const uint trial, uint& blacks, uint& whites, std::string& real_key);
  std::string quitGame(const std::string& plid, const time_t& cmd_tstamp);
  Game::Status getLastGame(const std::string& plid, const time_t& cmd_tstamp,
                           std::string& output);
  std::string getScoreboard();
};

#endif