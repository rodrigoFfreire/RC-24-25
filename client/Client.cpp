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
        tcp_cmd->second(game_state, tcp_socket, command_stream);
        return;
    }

    throw UnexpectedCommandException();
}

Client::Client(Config &config)
    : tcp_socket(config.ipaddr, config.port), udp_socket(config.ipaddr, config.port), game_state() {
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
        }

        handleCommand(command_id, command_stream);
    } catch (const UnexpectedCommandException& e) {
        std::cout << e.what() << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "ERROR: " << e.what() << std::endl;
    }
}

void Client::printHelp() {
    std::cout << "MasterMind: How to play?\n";
}