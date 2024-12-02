#ifndef CLIENT_GAME_STATE_HPP
#define CLIENT_GAME_STATE_HPP

#include <vector>
#include <array>
#include "../common/constants.hpp"

class GameState {
private:
    bool isGame;
    bool finished;
    bool _debug;
    unsigned int plid;
    unsigned int trial;
    std::vector<std::array<char, SECRET_KEY_LEN + 1>> guesses;
    std::vector<std::array<unsigned int, 2>> feedback;
    char _key[SECRET_KEY_LEN + 1] = {0};

    void registerColorMap();

public:
    GameState() : isGame(false), finished(false) {
        registerColorMap();
    };

    enum Events { LOST_TIME, LOST_MAXTRIALS, WON, QUIT };

    void startGame(unsigned int id, char *key, bool debug);
    void endGame(char *key, Events event);
    unsigned int getPlid();
    unsigned int newAttempt();
    unsigned int getTrial();
    void saveAttempt(char *att, unsigned int b, unsigned int w);
    void printState();
};

#endif