#ifndef CLIENT_GAME_STATE_HPP
#define CLIENT_GAME_STATE_HPP

#include <vector>
#include <array>
#include <iostream>
#include <cstring>
#include <unordered_map>
#include <fstream>
#include "../common/constants.hpp"
#include "exceptions/ClientExceptions.hpp"
#include "exceptions/CommandExceptions.hpp"

class GameState {
private:
    bool _isGame;
    bool _finished;
    bool _debug = false;
    bool _unicode;
    unsigned int trial;
    std::string plid;
    std::vector<std::string> guesses;
    std::vector<std::array<unsigned int, 2>> feedback;
    std::string _key;

    void registerColorMap();

public:
    GameState(bool unicode) : _isGame(false), _finished(false), _unicode(unicode) {
        _key.resize(SECRET_KEY_LEN, '\0');
        registerColorMap();
    };

    enum Events { LOST_TIME, LOST_MAXTRIALS, WON, QUIT };

    bool isGame() {return _isGame;};
    void startGame(std::string& plid, std::string* key);
    void endGame(std::string& key, Events event);
    std::string getPlid();
    unsigned int newAttempt();
    unsigned int getTrial();
    void saveAttempt(std::string& att, unsigned int b, unsigned int w);
    void saveFile(std::string& fname, std::string& fdata);
    void printState();
};

#endif