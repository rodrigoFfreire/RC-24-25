#include "GameState.hpp"

std::unordered_map<char, std::string> colorMap;

/// @brief Color char to colored character or emoji (if unicode mode is set)
void GameState::registerColorMap() {
  if (_unicode) {
    colorMap['R'] = "🔴";
    colorMap['G'] = "🟢";
    colorMap['B'] = "🔵";
    colorMap['Y'] = "🟡";
    colorMap['O'] = "🟠";
    colorMap['P'] = "🟣";
    colorMap['b'] = "⚫";
    colorMap['W'] = "⚪";
  } else {
    colorMap['R'] = "\033[38;5;196mR\033[0m";
    colorMap['G'] = "\033[38;5;46mG\033[0m";
    colorMap['B'] = "\033[38;5;51mB\033[0m";
    colorMap['Y'] = "\033[38;5;226mY\033[0m";
    colorMap['O'] = "\033[38;5;208mO\033[0m";
    colorMap['P'] = "\033[38;5;57mP\033[0m";
    colorMap['W'] = "X";
    colorMap['b'] = "X";
  }
}

/// @brief Setup up a new game (client side), keeping track of the game's status and
/// attempts
/// @param id Player ID
/// @param key Secret key (debug mode)
void GameState::startGame(std::string& id, std::string* key) {
  plid = id;
  trial = 1;
  _isGame = true;
  _finished = false;

  _key.clear();
  guesses.clear();
  feedback.clear();

  if (key != nullptr) {
    _key = *key;
    _debug = true;  // Set debug mode if key was given
  }

  printState();
}

/// @brief Ends the game (client side), updating the status and explains the ending reason
/// @param key Revealed secret key
/// @param event Ending event (WON, LOST_MAXTRIALS, LOST_MAXTIME, QUIT)
void GameState::endGame(std::string& key, Events event) {
  _key = key;  // Stores the revealed secret key to display it later

  _finished = true;
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

  _isGame = false;
  if (event != Events::WON) {
    std::cout << "The correct key was: ";
    for (size_t i = 0; i < SECRET_KEY_LEN; ++i) {
      std::cout << colorMap[_key[i]];
    }
    std::cout << std::endl;
  }
}

/// @brief Retrieves the associated PLID to the current active game
std::string GameState::getPlid() {
  if (!_isGame && !_finished) {
    throw UncontextualizedException();
  }
  return plid;
}

/// @brief Returns the next trial number if the game is active, else doesnt increment it
unsigned int GameState::newAttempt() { return (_isGame ? trial++ : trial); }

/// @brief Returns the current trial number
unsigned int GameState::getTrial() { return trial; }

/// @brief Saves an attempt to the game state, (key, n-whites, n-blacks)
/// @param att Attempt key
/// @param b N blacks (N on-position pegs)
/// @param w N whites (N off-position pegs)
void GameState::saveAttempt(std::string& att, unsigned int b, unsigned int w) {
  guesses.push_back(att);
  feedback.push_back({b, w});
}

/// @brief Saves data into a file and prints it
/// @param fname Target file
/// @param fdata Data to be saved
void GameState::saveFile(std::string& fname, std::string& fdata) {
  try {
    std::ofstream file(fname);
    file.exceptions(std::ofstream::failbit | std::ofstream::badbit);

    file << fdata;
    file.close();

    std::cout << fdata;  // Print the file
  } catch (const std::exception& e) {
    throw SaveFileError();
  }
}

/// @brief Prints the current game state of the client. Displays a table of all issued
/// attempts
void GameState::printState() {
  if (!_isGame) {
    return;
  }
  std::cout << "\033[2J\033[H";  // Clears the screen
  std::cout << "Trial\t\tBlacks\t\tWhites\t\tSecret Key:\t\t";
  if (_finished || _debug) {
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
    for (size_t a = 0; a < fb[0]; ++a) {
      std::cout << colorMap['b'];
    };
    std::cout << "\t\t";
    for (size_t a = 0; a < fb[1]; ++a) {
      std::cout << colorMap['W'];
    };
    std::cout << "\t\t            \t";
    if (fb[0] < SECRET_KEY_LEN && fb[1] < SECRET_KEY_LEN) {
      std::cout << '\t';
    }
    for (size_t j = 0; j < SECRET_KEY_LEN; ++j) {
      std::cout << colorMap[guess[j]] << '\t';
    }
    std::cout << "\n\n";
  }
  std::cout << std::endl;
}
