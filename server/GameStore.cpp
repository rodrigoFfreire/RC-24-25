#include "GameStore.hpp"

std::string gameModeToRepr(GameMode mode) {
    switch (mode) {
    case GameMode::PLAY:
        return "PLAY";
    case GameMode::DEBUG:
        return "DEBUG";
    default:
        throw InvalidGameModeException();
    }
}

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

std::string endingToRepr(Endings ending) {
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

Attempt::Attempt(std::string& att) : blacks(0), whites(0), time(0) {
    std::string prefix;
    std::istringstream stream(att);

    stream >> prefix;   // "T:""
    stream >> att_key;
    stream >> blacks;
    stream >> whites;
    stream >> time;
}

std::string Attempt::create(std::string &att, uint blacks, uint whites, time_t time) {
    std::ostringstream att_ss;
    att_ss << "T: " << att << ' ' << blacks << ' ' << whites << ' ' << time << '\n';

    return att_ss.str();
};

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

uint Game::parseAttempts(std::ifstream &file, std::string& att, std::string& last_att, bool& dup) {
    std::string attempt_line;
    uint atts = 0;
    while (std::getline(file, attempt_line)) {
        if (attempt_line[0] == 'T') {
            Attempt attempt(attempt_line);
            atts++;
            if (!att.compare(attempt.att_key)) {
                dup = true;
            }
            last_att = attempt.att_key;
        }
    }
    return atts;
}

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

std::string Game::create(std::string &plid, uint playTime, GameMode mode, time_t &cmd_tstamp, std::string& key) {
    std::ostringstream header_ss;

    header_ss << plid << ' ';
    header_ss << gameModeToRepr(mode)[0] << ' ';
    header_ss << key << ' ';
    header_ss << playTime << ' ';
    formatTimestamp(header_ss, &cmd_tstamp, TSTAMP_DATE_TIME_PRETTY);
    header_ss << ' ' << cmd_tstamp << '\n';

    return header_ss.str();
}

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

void GameStore::calculateAttempt(std::string &key, std::string &att, uint &whites, uint &blacks) {
    const std::string valid_colors = VALID_COLORS;
    std::array<int, strlen(VALID_COLORS)> key_count({0});
    std::array<int, strlen(VALID_COLORS)> attempt_count({0});

    // First pass: Count black pegs and build frequency arrays
    for (int i = 0; i < SECRET_KEY_LEN; ++i) {
        if (key[i] == att[i]) {
            blacks++;
        } else {
            // Increment the frequency of unmatched colors
            key_count[valid_colors.find(key[i])]++;
            attempt_count[valid_colors.find(att[i])]++;
        }
    }

    // Second pass: Count white pegs
    for (size_t i = 0; i < valid_colors.size(); i++) {
        whites += (key_count[i] < attempt_count[i]) ? key_count[i] : attempt_count[i];
    }
}

void GameStore::saveGameScore(std::string &plid, std::string& key, GameMode mode, time_t &win_tstamp, int used_atts, int used_time) {
    std::ostringstream score_fname;
    std::ostringstream score_header;

    int score = 701 + ((MAX_GUESSES - used_atts * used_atts) * 100) / MAX_GUESSES + ((PLAY_TIME_MAX - used_time) * 211) / PLAY_TIME_MAX;

    score_fname << score << '_' << plid << '_';
    formatTimestamp(score_fname, &win_tstamp, TSTAMP_TIME_DATE_PRETTY_);
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

GameStore::GameStore(std::string &dir) {
    storeDir = fs::read_symlink("/proc/self/exe").parent_path() / dir;
}

int GameStore::updateGameTime(std::string& plid, time_t& cmd_tstamp, std::string* revealed_key) {
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

std::string GameStore::createGame(std::string& plid, uint playTime, time_t& cmd_tstamp, std::string* key) {
    if (updateGameTime(plid, cmd_tstamp, nullptr) > 0) {
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

std::string GameStore::quitGame(std::string &plid, time_t &cmd_tstamp) {
    if (updateGameTime(plid, cmd_tstamp, nullptr) < 0) {
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

Game::Status GameStore::getLastGame(std::string &plid, time_t &cmd_tstamp, std::string &output) {
    Game game;
    fs::path game_path;
    std::ostringstream output_ss;

    if (updateGameTime(plid, cmd_tstamp, NULL) < 0) {
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

    if (game.status == Game::Status::ACT) {
        output_ss << "Status: Active | Secret code: ? ? ? ?\n";
    } else {
        output_ss << "Status: Finished | Secret code: " << game.key << '\n';
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

std::string GameStore::getScoreboard() {
    fs::path scores_path = storeDir / "SCORES";
    std::vector<fs::path> score_paths;

    std::ostringstream output_ss;
    output_ss << "\n-------------------- Mastermind Leaderboard - TOP " << SCOREBOARD_MAX_ENTRIES << " --------------------\n\n";
    output_ss << "                 SCORE PLAYER     CODE    NO TRIALS   MODE\n\n";

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

        std::sort(score_paths.begin(), score_paths.end(), std::greater<>());

        for (size_t i = 0; i < score_paths.size() && i < SCOREBOARD_MAX_ENTRIES; ++i) {
            std::ifstream file(score_paths[i]);
            LeaderboardEntry entry(file);
            file.close();

            output_ss << "             " << i + 1 << " -  " << entry.score << "  " << entry.plid;
            output_ss << "     " << entry.key << "        " <<  entry.used_atts << "       " << gameModeToRepr(entry.mode) << "\n";
        }
    } catch (const EmptyScoreboardException& e) {
        throw EmptyScoreboardException();
    } catch (const std::exception& e) {
        throw DBFilesystemError();
    }

    return output_ss.str();
}

uint GameStore::attempt(std::string &plid, std::string& att, uint trial, uint &blacks, uint &whites, time_t& cmd_tstamp, std::string& real_key) {
    int err = updateGameTime(plid, cmd_tstamp, &real_key);
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

    bool isDup;
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

    if (num_attempts == MAX_GUESSES - 1 && blacks != SECRET_KEY_LEN) {
        endGame(plid, Endings::LOST, cmd_tstamp, o_file, used_time);
        real_key = game.key;
        throw ExceededMaxTrialsException();
    } else if (blacks == SECRET_KEY_LEN) {
        endGame(plid, Endings::WIN, cmd_tstamp, o_file, used_time);
        saveGameScore(plid, game.key, game.mode, cmd_tstamp, num_attempts + 1, used_time);
    }

    return ++num_attempts;
}

void GameStore::endGame(std::string& plid, Endings reason, time_t& tstamp, std::ofstream& file, int used_time) {
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
        fs::create_directories(finished_path.parent_path());

        fs::rename(game_path, finished_path);        
    } catch (const fs::filesystem_error& e) {
        throw DBFilesystemError();
    }
}

std::string GameStore::findLastFinishedGame(std::string &plid) {
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

LeaderboardEntry::LeaderboardEntry(std::ifstream& file) {
    char mode_char;

    file >> score;
    file >> plid;
    file >> key;
    file >> used_atts;
    file >> mode_char;

    mode = charToGameMode(mode_char);
}
