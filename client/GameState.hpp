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
    bool _debug;
    bool _unicode;
    unsigned int plid;
    unsigned int trial;
    std::vector<std::array<char, SECRET_KEY_LEN + 1>> guesses;
    std::vector<std::array<unsigned int, 2>> feedback;
    char _key[SECRET_KEY_LEN + 1] = {0};

    void registerColorMap();

public:
    GameState(bool unicode) : _isGame(false), _finished(false), _unicode(unicode) {
        registerColorMap();
    };

    enum Events { LOST_TIME, LOST_MAXTRIALS, WON, QUIT };

    bool isGame() {return _isGame;};
    void startGame(unsigned int id, char *key, bool debug);
    void endGame(char *key, Events event);
    unsigned int getPlid();
    unsigned int newAttempt();
    unsigned int getTrial();
    void saveAttempt(char *att, unsigned int b, unsigned int w);
    void saveFile(std::string& fname, std::string& fdata);
    void printState();
};

#endif