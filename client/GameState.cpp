#include "GameState.hpp"
#include "../common/constants.hpp"
#include <iostream>
#include <cstring>
#include <unordered_map>

std::unordered_map<char, std::string> colorMap;

void GameState::registerColorMap() {
    //colorMap['R'] = "\033[38;5;196mR\033[0m";
    //colorMap['G'] = "\033[38;5;46mG\033[0m";
    //colorMap['B'] = "\033[38;5;51mB\033[0m";
    //colorMap['Y'] = "\033[38;5;226mY\033[0m";
    //colorMap['O'] = "\033[38;5;208mO\033[0m";
    //colorMap['P'] = "\033[38;5;57mP\033[0m";
    colorMap['R'] = "🔴";
    colorMap['G'] = "🟢";
    colorMap['B'] = "🔵";
    colorMap['Y'] = "🟡";
    colorMap['O'] = "🟠";
    colorMap['P'] = "🟣";
}

void GameState::startGame(unsigned int id, char *key, bool debug) {
    plid = id;
    trial = 1;
    isGame = true;
    finished = false;
    _debug = debug;

    memset(_key, 0, SECRET_KEY_LEN + 1);
    guesses.clear();
    feedback.clear();

    if (key != nullptr) {
        strncpy(_key, key, SECRET_KEY_LEN);
    }

    printState();
}

void GameState::endGame(char *key, Events event) {
    strncpy(_key, key, SECRET_KEY_LEN);

    finished = true;
    printState();

    switch (event) {
    case Events::LOST_MAXTRIALS:
        std::cout << "GAME LOST! :( " << "You've used all of your attempts!\n";
        break;
    case Events::LOST_TIME:
        std::cout << "GAME LOST! :( " << "You've ran out of time!\n";
        break;
    case Events::WON:
        std::cout << "GAME WON! :) Congratulations!!!\n";
        break;
    case Events::QUIT:
        std::cout << "GAME QUIT! :/\n";
    default:
        break;
    }

    isGame = false;
    if (event != Events::WON) {
        std::cout << "The correct key was: ";
        for (size_t i = 0; i < SECRET_KEY_LEN; ++i) {
            std::cout << colorMap[_key[i]];
        }
        std::cout << std::endl;
    }
}

unsigned int GameState::getPlid() {
    return plid;
}

unsigned int GameState::newAttempt() {
    return (isGame ? trial++ : trial);
}

unsigned int GameState::getTrial() {
    return trial;
}

void GameState::saveAttempt(char *att, unsigned int b, unsigned int w) {
    std::array<char, SECRET_KEY_LEN + 1> entry({0});
    std::copy(att, att + SECRET_KEY_LEN, entry.begin());
    guesses.push_back(entry);
    feedback.push_back({b, w});
}

void GameState::printState() {
    if (!isGame) {
        return;
    }
    std::cout << "\033[2J\033[H"; // Clears the screen
    std::cout << "Trial\t\tBlacks\t\tWhites\t\tSecret Key:\t\t";
    if (finished || _debug) {
        for (size_t i = 0; i < SECRET_KEY_LEN; ++i) {
            std::cout << colorMap[_key[i]] << '\t';
        }
    } else {
        for (size_t i = 0; i < SECRET_KEY_LEN; ++i) {
            std::cout << '?' << '\t';
        }
    }
    std::cout << "\n\n";

    for (size_t i = 0; i < guesses.size(); ++i) {
        const auto& guess = guesses[i];
        const auto& fb = feedback[i];

        std::cout << i + 1 << "\t\t";
        for (size_t a = 0; a < fb[0]; ++a) {std::cout << "⚫";};
        std::cout << "\t\t";
        for (size_t a = 0; a < fb[1]; ++a) {std::cout << "⚪";};
        std::cout << "\t\t            \t";
        if (fb[0] < SECRET_KEY_LEN && fb[1] < SECRET_KEY_LEN) {std::cout << '\t';}
        for (size_t j = 0; j < SECRET_KEY_LEN; ++j) {
            std::cout << colorMap[guess[j]] << '\t';
        }
        std::cout << "\n\n";
    }
    std::cout << std::endl;
}
