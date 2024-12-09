#include "GameStore.hpp"

std::string GameStore::generateKey() {
    std::string key(SECRET_KEY_LEN, '\0');
    size_t valid_chars_len = strlen(VALID_COLORS);

    std::ifstream urandom("/dev/urandom", std::ios::in | std::ios::binary);
    if (!urandom.is_open()) {
        throw OpenFileError();
    }

    urandom.read(&key[0], SECRET_KEY_LEN);
    if (!urandom) {
        throw ReadFileError();
    }

    for (size_t i = 0; i < SECRET_KEY_LEN; ++i) {
        key[i] = VALID_COLORS[static_cast<unsigned char>(key[i]) % valid_chars_len];
    }

    urandom.close();

    return key;
}

void GameStore::formatDateTime(std::stringstream& ss, std::time_t &tstamp, char sep, bool pretty) {
    std::tm* timeInfo = std::localtime(&tstamp);
    if (timeInfo) {
        if (pretty) {
            ss << std::put_time(timeInfo, "%Y-%m-%d") << sep;
            ss << std::put_time(timeInfo, "%H:%M:%S") << sep;
        } else {
            ss << std::put_time(timeInfo, "%Y%m%d") << sep;
            ss << std::put_time(timeInfo, "%H%M%S") << sep;
        }
    }
}

char GameStore::endingToStr(Endings ending) {
    switch (ending) {
    case Endings::LOST:
        return 'L';
    case Endings::QUIT:
        return 'Q';
    case Endings::TIMEOUT:
        return 'T';
    case Endings::WIN:
        return 'W';
    default:
        return 0;
    }
}

GameStore::GameStore(std::string &dir) {
    storeDir = fs::read_symlink("/proc/self/exe").parent_path() / dir;
}

int GameStore::updateGameTime(std::string& plid, std::time_t& cmd_tstamp) {
    std::stringstream ss;
    ss << plid;

    std::string game_fname = "GAME_" + ss.str() + ".txt";
    fs::path game_path = storeDir / "GAMES" / game_fname;

    if (!fs::exists(game_path)) return -1;
    
    std::ifstream i_file(game_path);
    if (!i_file.is_open()) {
        throw OpenFileError();
    }

    try {
        std::string header;
        std::getline(i_file, header);
        i_file.close();

        const char* buffer = header.c_str();
        int start_tstamp;
        int play_time;
        int remaining_time;

        sscanf(buffer, "%*d %*c %*s %d %*s %*s %d", &play_time, &start_tstamp);
        remaining_time = cmd_tstamp - static_cast<std::time_t>(start_tstamp);

        if (remaining_time >= play_time) {
            std::ofstream o_file(game_path, std::ios::app);
            endGame(plid, Endings::TIMEOUT, cmd_tstamp, o_file, play_time);
            return -1;
        }
        return remaining_time;
    } catch (const std::exception& e) {
        throw ReadFileError();
    }
}

std::string GameStore::createGame(std::string& plid, unsigned short playTime, GameMode gamemode, std::time_t& cmd_tstamp) {
    try {
        if (updateGameTime(plid, cmd_tstamp) != -1) {
            throw OngoingGameException();
        }
    } catch (const std::exception& e) {
        throw OngoingGameException();
    }

    std::stringstream game_header;
    std::string key = generateKey();

    game_header << plid << ' ';
    switch (gamemode) {
    case PLAY:
        game_header << "P ";
        break;
    case DEBUG:
        game_header << "D ";
        break;
    default:
        throw InvalidGameModeException();
    }

    game_header << key << ' ';
    game_header << playTime << ' ';
    formatDateTime(game_header, cmd_tstamp, ' ', true);
    game_header << cmd_tstamp << '\n';

    std::string game_fname = "GAME_" + plid + ".txt";
    fs::path game_path = storeDir / "GAMES" / game_fname;

    std::ofstream file(game_path, std::ios::trunc);
    if (!file.is_open()) {
        throw OpenFileError();
    }

    file << game_header.str();

    return key;
}

void GameStore::endGame(std::string& plid, Endings reason, std::time_t& tstamp, std::ofstream& file, int used_time) {
    std::stringstream ss;
    formatDateTime(ss, tstamp, ' ', true);
    ss << used_time << '\n';

    file << ss.str();
    file.close();

    std::string game_fname = "GAME_" + plid + ".txt";
    fs::path game_path = storeDir / "GAMES" / game_fname;

    std::stringstream finished_fname;
    formatDateTime(finished_fname, tstamp, '_', false);
    finished_fname << endingToStr(reason) << ".txt";

    fs::path finished_path = storeDir / "GAMES" / plid / finished_fname.str();

    try {
        fs::create_directories(finished_path.parent_path());

        fs::rename(game_path, finished_path);        
    } catch (const fs::filesystem_error& e) {
        throw MoveFileError();
    }
}
