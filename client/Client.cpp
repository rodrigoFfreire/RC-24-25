#include "Client.hpp"
#include <algorithm>
#include <cctype>
#include "commands/udp_commands.hpp"
#include "commands/tcp_commands.hpp"

void Client::registerCommands() {
    udp_handlers.insert({"start", startNewGameHandler});
    udp_handlers.insert({"try", tryHandler});
    udp_handlers.insert({"quit", quitHandler});
    udp_handlers.insert({"debug", debugGameHandler});

    tcp_handlers.insert({"show_trials", showTrialsHandler});
    tcp_handlers.insert({"st", showTrialsHandler});
    tcp_handlers.insert({"scoreboard", showScoreboardHandler});
    tcp_handlers.insert({"sb", showScoreboardHandler});
}

void Client::handleCommand(std::string& command_id, std::stringstream& command_stream) {
    auto udp_cmd = udp_handlers.find(command_id);
    if (udp_cmd != udp_handlers.end()) {
        udp_cmd->second(game_state, udp_socket, command_stream);
        return;
    }

    auto tcp_cmd = tcp_handlers.find(command_id);
    if (tcp_cmd != tcp_handlers.end()) {
        tcp_socket.setup();
        tcp_cmd->second(game_state, tcp_socket);
        tcp_socket.end();
        return;
    }

    throw UnexpectedCommandException();
}

Client::Client(Config &config)
    : tcp_socket(config.ipaddr, config.port), udp_socket(config.ipaddr, config.port),
        game_state(config.unicode), unicode(config.unicode) {
    registerCommands();
    udp_socket.setup();
}

void Client::getNextCommand() {
    try {
        std::string line;
        std::stringstream command_stream;

        std::cout << "MasterMind > ";

        std::getline(std::cin, line);
        if (std::cin.eof()) {
            return;
        }

        // Put obtained line in a stream and get command id
        command_stream << line;
        std::string command_id;
        command_stream >> command_id;

        std::transform(command_id.begin(), command_id.end(), command_id.begin(),
                   [](unsigned char c) { return std::tolower(c); });

        if (command_id == "exit") {
            exit = true;
            if (game_state.isGame())
                quitHandler(game_state, udp_socket, command_stream);
            return;
        } else if (command_id == "?") {
            printHelp();
            return;
        }

        handleCommand(command_id, command_stream);
    } catch (const UnexpectedCommandException& e) {
        std::cout << e.what() << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "ERROR: " << e.what() << std::endl;
    }
}

void Client::printHelp() {
    std::cout << "\033[2J\033[H";
    std::cout << "MasterMind: Commands\n\n";
    std::cout << "-> start <PLID> <MAXTIME>: Starts a new game. PLID must be [0, 999999] and MAXTIME must not exceed 600\n";
    std::cout << "-> try <key>. Given an ongoing game, sends an attempt with that key. Check the key format below\n";
    std::cout << "-> show_trials, st: Shows information about your last played game. Must have an ongoing or finished game to have an associated PLID\n";
    std::cout << "-> scoreboard, sb: Shows the TOP 10 players and their corresponding results\n";
    std::cout << "-> debug <PLID> <MAXTIME> <key>: Starts a new debug game where you define the secret key\n";
    std::cout << "-> quit: Quits an ongoing game but not the player\n";
    std::cout << "-> exit: Exits the player\n\n";

    std::cout << "MasterMind: Secret Key\n\n";
    std::cout << "The secret key is composed of 4 colors (can be repeated) identified by their first letter\n";
    std::cout << "The key can be specified with or without whitespace in between each character!\n";
    std::cout << "The valid colors are:\n";
    if (unicode) {
        std::cout << "\t- Red (🔴) - R or r\n";
        std::cout << "\t- Green (🟢) - G or g\n";
        std::cout << "\t- Blue (🔵) - B or b\n";
        std::cout << "\t- Yellow (🟡) - Y or y\n";
        std::cout << "\t- Orange (🟠) - O or o\n";
        std::cout << "\t- Purple (🟣) - P or p\n\n";
    } else {
        std::cout << "\t- Red - R or r\n";
        std::cout << "\t- Green - G or g\n";
        std::cout << "\t- Blue - B or b\n";
        std::cout << "\t- Yellow - Y or y\n";
        std::cout << "\t- Orange - O or o\n";
        std::cout << "\t- Purple - P or p\n\n";
    }
    std::cout << "Examples:\n";
    std::cout << "\t- try GROG\n";
    std::cout << "\t- try B B B B\n";
    std::cout << "\t- debug 106485 337 boby\n\n";
    std::cout << "Press ENTER to continue..." << std::endl;

    std::cin.get();
    std::cout << "\033[2J\033[H" << std::endl;
}