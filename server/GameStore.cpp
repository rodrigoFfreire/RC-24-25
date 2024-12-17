#include <fstream>
#include <vector>
#include <algorithm>

#include "GameStore.hpp"
#include "exceptions/GameExceptions.hpp"
#include "exceptions/ServerExceptions.hpp"
#include "../common/utils.hpp"
#include "../common/constants.hpp"


namespace fs = std::filesystem;

/// @brief Returns the string representation of a GameMode
/// @param mode  Gamemode
std::string gameModeToRepr(const GameMode mode) {
    switch (mode) {
    case GameMode::PLAY:
        return "PLAY";
    case GameMode::DEBUG:
        return "DEBUG";
    default:
        throw InvalidGameModeException();
    }
}

/// @brief Returns the GameMode given the char representation
/// @param c  Gamemode char
GameMode charToGameMode(const char c) {
    switch (c) {
    case 'P':
        return GameMode::PLAY;
    case 'D':
        return GameMode::DEBUG;
    default:
        throw InvalidGameModeException();
    }
}

/// @brief Returns the string representation of an Ending
/// @param ending Ending
std::string endingToRepr(const Endings ending) {
    switch (ending) {
    case Endings::LOST:
        return "LOST";
    case Endings::WIN:
        return "WIN";
    case Endings::QUIT:
        return "QUIT";
    case Endings::TIMEOUT:
        return "TIMEOUT";
    default:
        throw InvalidEndingException();
    }
}

/// @brief Returns the Ending given the char representation
/// @param c Ending char 
Endings charToEnding(const char c) {
    switch (c) {
    case 'L':
        return Endings::LOST;
    case 'W':
        return Endings::WIN;
    case 'Q':
        return Endings::QUIT;
    case 'T':
        return Endings::TIMEOUT;
    default:
        throw InvalidEndingException();
    }
}

/// @brief Attempt object constructor.
/// @param att Attempt in string format (Ex: T: GROG 4 0 123)
Attempt::Attempt(const std::string& att) : blacks(0), whites(0), time(0) {
    std::string prefix;
    std::istringstream stream(att);

    stream >> prefix;   // "T:"
    stream >> att_key;
    stream >> blacks;
    stream >> whites;
    stream >> time;
}

/// @brief Serializes an attempt given its parameters
/// @param key Attempt key
/// @param blacks N blacks (On-position pegs)
/// @param whites N whites (Off-position pegs)
/// @param time Elapsed time
/// @return Serialized attempt in string format
std::string Attempt::create(const std::string &key, const uint blacks, const uint whites, const time_t time) {
    std::ostringstream att_ss;
    att_ss << "T: " << key << ' ' << blacks << ' ' << whites << ' ' << time << '\n';

    return att_ss.str();
};

/// @brief Parses the header of game file. (Ex: 100001 D RGBY 100 2024-12-16 19:12:36 1734376356)
/// @param file Game file
void Game::parseHeader(std::ifstream &file) {
    std::string header;
    std::getline(file, header);
    std::istringstream stream(header);
    char mode_char;

    stream >> plid;
    stream >> mode_char;
    stream >> key;
    stream >> playTime;
    stream >> date_start;
    stream >> time_start;
    stream >> tstamp_start;

    mode = charToGameMode(mode_char);
}

/// @brief Parses all attempts of a given file. Checks for duplicate attempts
/// @param file Game file
/// @param key The new attempt's key
/// @param last_att Will store the last attempt's key according to the file
/// @param dup Will indicate if the new attempt's key is duplicated
/// @return The number of stored attempts in the file
uint Game::parseAttempts(std::ifstream &file, const std::string& key, std::string& last_att, bool& dup) {
    std::string attempt_line;
    uint atts = 0;
    while (std::getline(file, attempt_line)) {
        if (attempt_line[0] == 'T') {
            Attempt attempt(attempt_line);
            atts++;
            if (!key.compare(attempt.att_key)) {
                dup = true;
            }
            last_att = attempt.att_key;
        }
    }
    return atts;
}

/// @brief Parses an entire Game file
/// @param file Game file 
void Game::parseGame(std::ifstream &file) {
    std::string attempt_line;
    parseHeader(file);

    while (std::getline(file, attempt_line)) {
        if (attempt_line[0] == 'T') {
            Attempt attempt(attempt_line);
            attempts.push_back(attempt);
        } else {
            std::istringstream end_stream(attempt_line);
            char mode_char;

            end_stream >> date_end;
            end_stream >> time_end;
            end_stream >> usedTime;
            end_stream >> mode_char;

            ending = charToEnding(mode_char);
        }
    }
}

/// @brief Creates a serialized game header
/// @param plid Player ID
/// @param playTime Maximum play time
/// @param mode Gamemode (Play | Debug)
/// @param cmd_tstamp Timestamp of created game
/// @param key Secret key
/// @return The new game's header in string format
std::string Game::create(const std::string &plid, const uint playTime, const GameMode mode, const time_t &cmd_tstamp, const std::string& key) {
    std::ostringstream header_ss;

    header_ss << plid << ' ';
    header_ss << gameModeToRepr(mode)[0] << ' ';
    header_ss << key << ' ';
    header_ss << playTime << ' ';
    formatTimestamp(header_ss, &cmd_tstamp, TSTAMP_DATE_TIME_PRETTY);
    header_ss << ' ' << cmd_tstamp << '\n';

    return header_ss.str();
}

/// @brief Uses /dev/urandom to generate a random secret key
/// @return Secret key
std::string GameStore::generateKey() {
    std::string key(SECRET_KEY_LEN, '\0');
    size_t valid_chars_len = strlen(VALID_COLORS);

    std::ifstream urandom("/dev/urandom", std::ios::in | std::ios::binary);
    if (!urandom.is_open()) {
        throw DBFilesystemError();
    }

    urandom.read(&key[0], SECRET_KEY_LEN);
    if (!urandom) {
        throw DBFilesystemError();
    }

    for (size_t i = 0; i < SECRET_KEY_LEN; ++i) {
        key[i] = VALID_COLORS[static_cast<unsigned char>(key[i]) % valid_chars_len];
    }

    urandom.close();
    return key;
}

/// @brief Calculates the on-position and off-position pegs of a given attempt
/// @param key Game secret key
/// @param att Attempt key
/// @param whites N Off-position pegs  
/// @param blacks N On-position pegs
void GameStore::calculateAttempt(const std::string &key, const std::string &att, uint &whites, uint &blacks) {
    const std::string valid_colors = VALID_COLORS;

    uint key_count[VALID_COLORS_LEN] = {0};
    uint attempt_count[VALID_COLORS_LEN] = {0};

    for (size_t i = 0; i < SECRET_KEY_LEN; ++i) {
        if (key[i] == att[i]) {
            // Count on-position pegs
            blacks++;
        } else {
            // Increment frequency of off-position colors
            key_count[valid_colors.find(key[i])]++;
            attempt_count[valid_colors.find(att[i])]++;
        }
    }

    // Count off-position pegs
    for (size_t i = 0; i < valid_colors.size(); i++) {
        whites += (key_count[i] < attempt_count[i]) ? key_count[i] : attempt_count[i];
    }
}

/// @brief Calculates the score of a won game, and saves it to the SCORES directory
/// @param plid Player ID
/// @param key Secret key
/// @param mode Game mode
/// @param win_tstamp Timestamp of win
/// @param used_atts N used attempts
/// @param used_time used time (seconds)
void GameStore::saveGameScore(const std::string &plid, const std::string& key, const GameMode mode, const time_t &win_tstamp, const int used_atts, const int used_time) {
    std::ostringstream score_fname;
    std::ostringstream score_header;

    int score = 701 + ((GUESSES_MAX - used_atts * used_atts) * 100) / GUESSES_MAX + ((PLAY_TIME_MAX - used_time) * 211) / PLAY_TIME_MAX;

    score_fname << score << '_' << plid << '_';
    formatTimestamp(score_fname, &win_tstamp, TSTAMP_DATE_TIME_PRETTY_);
    score_fname << ".txt";

    score_header << score << ' ' << plid << ' ' << key << ' ';
    score_header << used_atts << ' ' << gameModeToRepr(mode)[0] << '\n';

    fs::path score_path = storeDir / "SCORES" / score_fname.str();
    std::ofstream file(score_path);
    if (!file.is_open()) {
        throw DBFilesystemError();
    }

    try {
        file << score_header.str();
        file.close();
    } catch (const std::exception& e) {
        throw DBFilesystemError();
    }
}

/// @brief Initializes the required directories for the database
/// @param dir 
GameStore::GameStore(const std::string &dir) {
    storeDir = fs::current_path() / dir;

    fs::create_directory(storeDir);
    fs::create_directory(storeDir / "GAMES");
    fs::create_directory(storeDir / "SCORES");
}

/// @brief Checks if a given player already has an open game, and closes it if ended already
/// @param plid Player ID
/// @param cmd_tstamp Command activation timestamp
/// @param revealed_key If necessary updated this pointer with the secret key
/// @return `-1`: No active game;   `-2`: Game ended by timeout;    Else: remaining time to play (seconds)
int GameStore::checkTimedoutGame(const std::string& plid, const time_t& cmd_tstamp, std::string* revealed_key) {
    std::string game_fname = "GAME_" + plid + ".txt";
    fs::path game_path = storeDir / "GAMES" / game_fname;

    if (!fs::exists(game_path)) return -1;
    
    std::ifstream file(game_path);
    if (!file.is_open()) {
        throw DBFilesystemError();
    }

    try {
        Game game;
        game.parseHeader(file);
        file.close();

        int remaining_time = static_cast<int>(cmd_tstamp) - game.tstamp_start;

        if (remaining_time >= static_cast<int>(game.playTime)) {
            time_t end_tstamp = cmd_tstamp + static_cast<time_t>(game.playTime);
            if (revealed_key != nullptr) {
                *revealed_key = game.key;
            }

            std::ofstream o_file(game_path, std::ios::app);
            endGame(plid, Endings::TIMEOUT, end_tstamp, o_file, game.playTime);
            return -2;
        }
        return remaining_time;
    } catch (const std::exception& e) {
        throw DBFilesystemError();
    }
}

/// @brief Creates a new game entry given the arguments
/// @param plid Player ID
/// @param cmd_tstamp Command activation timestamp
/// @param playTime Max time allowed to play
/// @param key Secret key
/// @return Returns the secret key. (Used for logging purposes)
std::string GameStore::createGame(const std::string& plid, const time_t& cmd_tstamp, const uint playTime, std::string* key) {
    // Active game exists
    if (checkTimedoutGame(plid, cmd_tstamp, nullptr) > 0) {
        throw OngoingGameException();
    }

    std::string new_key;
    GameMode mode;
    if (key == nullptr) {
        new_key = generateKey();
        mode = GameMode::PLAY;
    } else {
        new_key = *key;
        mode = GameMode::DEBUG;
    }

    std::string game_header = Game::create(plid, playTime, mode, cmd_tstamp, new_key);

    std::string game_fname = "GAME_" + plid + ".txt";
    std::ofstream file(storeDir / "GAMES" / game_fname, std::ios::trunc);
    if (!file.is_open()) {
        throw DBFilesystemError();
    }

    try {
        file << game_header;
        file.close();
    } catch (const std::exception& e) {
        throw DBFilesystemError();
    }

    return new_key;
}

/// @brief Quits the game
/// @param plid Player ID
/// @param cmd_tstamp Command activation timestamp
/// @return Revealed secret key
std::string GameStore::quitGame(const std::string &plid, const time_t &cmd_tstamp) {
    // No ongoing game
    if (checkTimedoutGame(plid, cmd_tstamp, nullptr) < 0) {
        throw UncontextualizedGameException();
    }

    std::string game_fname = "GAME_" + plid + ".txt";
    fs::path game_path = storeDir / "GAMES" / game_fname;

    std::ifstream i_file(game_path);
    if (!i_file.is_open()) {
        throw DBFilesystemError();
    }

    Game game;
    game.parseHeader(i_file);

    std::ofstream o_file(game_path, std::ios::app);
    if (!o_file.is_open()) {
        throw DBFilesystemError();
    }

    endGame(plid, Endings::QUIT, cmd_tstamp, o_file, cmd_tstamp - game.tstamp_start);

    return game.key;
}

/// @brief Retrieves the last game (active/finished) and creates a formatted file with all information about it
/// @param plid Player ID
/// @param cmd_tstamp Command activation timestamp
/// @param output Output file containing all information about the game
/// @return Game status (Active | Finished)
Game::Status GameStore::getLastGame(const std::string &plid, const time_t &cmd_tstamp, std::string &output) {
    Game game;
    fs::path game_path;
    std::ostringstream output_ss;

    if (checkTimedoutGame(plid, cmd_tstamp, NULL) < 0) {
        game_path = storeDir / "GAMES" / plid / findLastFinishedGame(plid);
        game.status = Game::Status::FIN;
    } else {
        game_path = storeDir / "GAMES" / std::string("GAME_" + plid + ".txt");
        game.status = Game::Status::ACT;
    }

    std::ifstream file(game_path);
    if (!file.is_open()) {
        throw DBFilesystemError();
    }

    try {
        game.parseGame(file);
        file.close();
    } catch (const std::exception& e) {
        throw DBFilesystemError();
    }

    output_ss << "\nPlayer: " << plid << " | Mode: " << gameModeToRepr(game.mode) << '\n';
    output_ss << "Status: ";

    if (game.status == Game::Status::ACT) {
        output_ss << "Active | Secret code: ";
        if (game.mode == GameMode::DEBUG) {
            output_ss << game.key << '\n';
        } else {
            output_ss << "????" << '\n'; 
        }
    } else {
        output_ss << "Finished | Secret code: " << game.key << '\n';
    }

    output_ss << "Game initiated: " << game.date_start << ' ' << game.time_start << ' ';
    output_ss << "with " << game.playTime << "s to be completed\n";

    if (game.attempts.empty()) {
        output_ss << "\n    --- No transactions found ---\n\n";
    } else {
        output_ss << "\n    --- Transactions found: " << game.attempts.size() << " ---\n\n";
    }

    for (const auto& att: game.attempts) {
        output_ss << "    Trial: " << att.att_key << " | nB: " << att.blacks << " | nW: " << att.whites;
        output_ss << " at " << att.time << "s\n";
    }

    if (game.status == Game::Status::ACT) {
        output_ss << "\n    --- " << (game.playTime - (cmd_tstamp - game.tstamp_start)) << " seconds remaining ---\n";
    } else {
        output_ss << "\nGame terminated: " << endingToRepr(game.ending) << " at " << game.date_end << ' ' << game.time_end;
        output_ss << "\nDuration: " << game.usedTime << " seconds\n";
    }

    output = output_ss.str();
    return game.status;
}

/// @brief Retrieves the TOP N scores from the SCORES dir and creates a formatted scoreboard
/// @return Scoreboard output
std::string GameStore::getScoreboard() {
    fs::path scores_path = storeDir / "SCORES";
    std::vector<fs::path> score_paths;

    std::ostringstream output_ss;
    output_ss << "\n----------------- Mastermind Leaderboard - TOP " << SCOREBOARD_MAX_ENTRIES << " -----------------\n\n";
    output_ss << "        \tSCORE PLAYER     CODE    NO TRIALS   MODE\n\n";

    try {
        for (const auto& entry : fs::directory_iterator(scores_path)) {
            if (!entry.is_regular_file() || entry.path().extension() != ".txt") {
                continue;
            }
            score_paths.push_back(entry.path());
        }

        if (score_paths.empty()) {
            throw EmptyScoreboardException();
        }

        // Sorts the files in alphabetical descending order
        std::sort(score_paths.begin(), score_paths.end(), std::greater<>());

        for (size_t i = 0; i < score_paths.size() && i < SCOREBOARD_MAX_ENTRIES; ++i) {
            std::ifstream file(score_paths[i]);
            LeaderboardEntry entry(file);
            file.close();

            output_ss << "        " << i + 1 << "\t " << entry.score << "  " << entry.plid;
            output_ss << "     " << entry.key << "        " <<  entry.used_atts << "       " << gameModeToRepr(entry.mode) << "\n";
        }
    } catch (const EmptyScoreboardException& e) {
        throw EmptyScoreboardException();
    } catch (const std::exception& e) {
        throw DBFilesystemError();
    }

    return output_ss.str();
}

/// @brief Registers an attempt to an ongoing game
/// @param plid Player ID
/// @param cmd_tstamp Command activation timestamp
/// @param att Attempt's key
/// @param trial Trial number
/// @param blacks Will store the N of on-position pegs
/// @param whites Will store the N of off-position pegs
/// @param real_key Will store the game's key
/// @return The received trial number + 1
uint GameStore::attempt(const std::string &plid, const time_t& cmd_tstamp, const std::string& att, const uint trial, uint &blacks, uint &whites, std::string& real_key) {
    int err = checkTimedoutGame(plid, cmd_tstamp, &real_key);
    if (err == -1) {
        throw UncontextualizedGameException();
    } else if (err == -2) {
        throw TimedoutGameException();
    }

    std::string game_fname = "GAME_" + plid + ".txt";
    fs::path game_path = storeDir / "GAMES" / game_fname;
    
    std::ifstream i_file(game_path, std::ios::in | std::ios::out);
    if (!i_file.is_open()) {
        throw DBFilesystemError();
    }

    bool isDup = false;
    std::string last_att;
    uint num_attempts = 0;
    Game game;

    try {
        game.parseHeader(i_file);

        num_attempts = game.parseAttempts(i_file, att, last_att, isDup);
        i_file.close();
    } catch (const std::exception& e) {
        throw DBFilesystemError();
    }

    if (trial == num_attempts && !att.compare(last_att)) {
        calculateAttempt(game.key, att, whites, blacks);
        return num_attempts; // Client sent the same trial
    } 
    else if ((trial == num_attempts && att.compare(last_att)) ||
                trial != num_attempts + 1) {
        throw InvalidTrialException();
    } else if (trial == num_attempts + 1 && isDup) {
        throw DuplicateTrialException();
    }

    time_t used_time = cmd_tstamp - game.tstamp_start;
    calculateAttempt(game.key, att, whites, blacks);

    std::ofstream o_file(game_path, std::ios::app);

    try {
        o_file << Attempt::create(att, blacks, whites, used_time);
    } catch (const std::exception& e) {
        throw DBFilesystemError();
    }

    if (num_attempts == GUESSES_MAX - 1 && blacks != SECRET_KEY_LEN) {
        endGame(plid, Endings::LOST, cmd_tstamp, o_file, used_time);
        real_key = game.key;
        throw ExceededMaxTrialsException();
    } else if (blacks == SECRET_KEY_LEN) {
        endGame(plid, Endings::WIN, cmd_tstamp, o_file, used_time);
        saveGameScore(plid, game.key, game.mode, cmd_tstamp, num_attempts + 1, used_time);
    }

    return ++num_attempts;
}

/// @brief Ends the game with a given reason
/// @param plid Player ID
/// @param reason Ending reason (WIN, LOSS, QUIT, TIMEOUT)
/// @param tstamp Command activation timestamp
/// @param file Opened target game file
/// @param used_time Total used time for this game (seconds)
void GameStore::endGame(const std::string& plid, const Endings reason, const time_t& tstamp, std::ofstream& file, const int used_time) {
    std::ostringstream ss;
    formatTimestamp(ss, &tstamp, TSTAMP_DATE_TIME_PRETTY);
    ss << ' ' << used_time << ' ' << endingToRepr(reason)[0] << '\n';

    file << ss.str();
    file.close();

    std::string game_fname = "GAME_" + plid + ".txt";
    fs::path game_path = storeDir / "GAMES" / game_fname;

    std::ostringstream finished_fname;
    formatTimestamp(finished_fname, &tstamp, TSTAMP_DATE_TIME_);
    finished_fname << '_' << endingToRepr(reason)[0] << ".txt";

    fs::path finished_path = storeDir / "GAMES" / plid / finished_fname.str();

    try {
        // Create PLID directory and store the finished game there
        fs::create_directories(finished_path.parent_path());

        fs::rename(game_path, finished_path);        
    } catch (const fs::filesystem_error& e) {
        throw DBFilesystemError();
    }
}

/// @brief Finds the last finished game of a given player
/// @param plid Player ID
/// @return The target game file name
std::string GameStore::findLastFinishedGame(const std::string &plid) {
    fs::path dir = storeDir / "GAMES" / plid;
    std::vector<std::string> filenames;

    if (!fs::exists(dir)) {
        throw NeverPlayedException();
    }

    try {
        for (const auto& entry : fs::directory_iterator(dir)) {
            if (entry.is_regular_file() && entry.path().extension() == ".txt") {
                filenames.push_back(entry.path().filename().string());
            }
        }

        std::sort(filenames.begin(), filenames.end(), std::greater<>());
    } catch (const std::exception& e) {
        throw DBFilesystemError();
    }

    if (filenames.empty()) {
        throw NeverPlayedException();
    }

    return filenames[0];
}

/// @brief Leaderboard entry object constructor
/// @param file Score file
LeaderboardEntry::LeaderboardEntry(std::ifstream& file) {
    char mode_char;

    file >> score;
    file >> plid;
    file >> key;
    file >> used_atts;
    file >> mode_char;

    mode = charToGameMode(mode_char);
}
