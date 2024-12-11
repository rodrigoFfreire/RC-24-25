#ifndef COMMON_CONSTANTS_HPP
#define COMMON_CONSTANTS_HPP

#define DEFAULT_IPADRR              "127.0.0.1"
#define DEFAULT_PORT                "58065"
#define DEAFULT_PLID                "106485"
#define DEFAULT_DATA_PATH           ".data"

#define LOGGER_LEVEL_INFO           "INFO"
#define LOGGER_LEVEL_WARNING        "WARN"
#define LOGGER_LEVEL_ERROR          "ERR"

#define TSTAMP_DATE_TIME_PRETTY     "%Y-%m-%d %H:%M:%S"
#define TSTAMP_DATE_TIME            "%Y%m%d %H%M%S"
#define TSTAMP_DATE_TIME_           "%Y%m%d_%H%M%S"
#define TSTAMP_TIME_DATE_PRETTY_    "%H:%M:%S_%Y-%m-%d"
#define TSTAMP_TIME_DATE            "%H%M%S %Y%m%d"

#define VALID_COLORS                "RGBYOP"

#define SOCK_BUFFER_SIZE            32
#define SERVER_RECV_TIMEOUT         5
#define CLIENT_RECV_TIMEOUT         10
#define CLIENT_SEND_TIMEOUT         10
#define TCP_BACKLOG                 5
#define TCP_MAXCLIENTS              5
#define TCP_CONN_RECV_TIMEOUT       5
#define TCP_CONN_SEND_TIMEOUT       5
#define PLID_MAX                    999999

#define SECRET_KEY_LEN              4
#define PACKET_ID_LEN               3
#define STATUS_CODE_LEN             3
#define PLAYER_ID_LEN               6
#define PLAY_TIME_MAX               600
#define MAX_GUESSES                 8

#define FNAME_MAX                   24
#define FSIZE_MAX                   2048
#define FSIZE_STR_MAX               4

#endif