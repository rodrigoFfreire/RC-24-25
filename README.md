# Redes de Computadores 2024/2025 _(Computer Networks)_

# Project Requirements
The goal of this project was to develop a game **server** application for playing [_Mastermind_](https://en.wikipedia.org/wiki/Mastermind_(board_game)) using the socket interface, designed to handle both TCP and UDP connections. Additionally, a **client** application was developed to interact seamlessly with the server. For more details, be sure to review the project [requirements](./project_requirements.pdf).

# Dependencies
- Makefile
- C++17

Run `make` to compile the `server` and `player` binaries.

# Top-level structure
```
.
├── client     <- Client related code
│
├── common     <- Common code (protocol, utilities, etc...)
│
└── server     <- Server related code
```

# Server
```
Usage: ./GS [-p <GSport>] [-v] [-h]
Options:
	-p <GSport>	 Sets Game server port
	-v		     Enables verbose mode
	-h		     Displays this usage message
```

**Verbose mode**: displays the raw packets sent and received for debugging purposes. A severity-based logging feature has been added with respective color coding and timestamping for cleaner and more readable log activity.

The server utilizes both TCP and UDP protocols for handling specific commands, with each listener running in a separate thread.

- **UDP Requests:** Managed sequentially, as these operations are relatively inexpensive.
- **TCP Requests:** Concurrency is handled using a fixed-size thread pool (adjustable via the `TCP_MAXCLIENTS` constant in [constants.hpp](./common/constants.hpp)). Each connection is queued and managed by an available worker thread. While the queue itself has no size limit, the `TCP_BACKLOG` constant defines the maximum number of simultaneous connection requests.

Game data is stored in the `.data` directory located in the root of the project.

The server supports graceful termination through a SIGINT (`^C`) or SIGTERM signal.

Socket-level timeouts are implemented and configurable through the [constants.hpp](./common/constants.hpp) file.

# Client
```
Usage: ./player [-n <GSIP>] [-p <GSport>] [-u] [-h]
Options:
	-n <GSIP>	 Sets Game server IP address
	-p <GSport>	 Sets Game server port
	-u		     Unicode mode. Uses emojis to render the colors (Recommended)
	-h		     Displays this usage message
```

**Unicode Mode:** This feature enables the client to render the colored pegs as emojis. Be sure to activate this option when running the player if your terminal emulator supports emojis.

After launching the player, you can use the `?` command to display instructions on how to play the game.

The client also saves the current game state as you play, allowing it to provide a visualization of your progress. This feature is especially helpful for keeping track of your attempts.

# Other considerations

The protocol structure was designed with modularity in mind, making it easy to add new packets and commands by creating new derived classes from `UdpPacket` and `TcpPacket`

Many other configurations can be adjusted in the [constants.hpp](./common/constants.hpp) file such as:
- Maximum number of guesses
- Scoreboard entries
- Secret key length
- Allowed colors