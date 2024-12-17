#ifndef COMMON_CONSTANTS_HPP
#define COMMON_CONSTANTS_HPP

// Default configuration values
#define DEFAULT_IPADRR              "127.0.0.1"
#define DEFAULT_PORT                "58065"
#define DEFAULT_DATA_PATH           ".data"

// Timestamp formatting string
#define TSTAMP_DATE_TIME_PRETTY     "%Y-%m-%d %H:%M:%S"
#define TSTAMP_DATE_TIME_PRETTY_    "%Y-%m-%d_%H:%M:%S"
#define TSTAMP_DATE_TIME_           "%Y%m%d_%H%M%S"

// General game configurations
#define VALID_COLORS                "RGBYOP"
#define VALID_COLORS_LEN            6
#define PLID_MAX                    999999
#define PLID_LEN                    6
#define SECRET_KEY_LEN              4
#define PACKET_ID_LEN               3
#define STATUS_CODE_LEN             3
#define PLAY_TIME_MAX               600
#define GUESSES_MAX                 8
#define SCOREBOARD_MAX_ENTRIES      10

// Server TCP settings
#define SOCK_BUFFER_SIZE            32
#define TCP_BACKLOG                 5
#define TCP_MAXCLIENTS              5
#define TCP_CONN_RECV_TIMEOUT       5
#define TCP_CONN_SEND_TIMEOUT       5

// Server UDP settings
#define SERVER_RECV_TIMEOUT         5

// Client settings
#define CLIENT_RECV_TIMEOUT         10
#define CLIENT_SEND_TIMEOUT         10

// Filesystem settings
#define FNAME_MAX                   24
#define FSIZE_MAX                   2048
#define FSIZE_STR_MAX               4

#endif