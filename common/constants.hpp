#ifndef COMMON_CONSTANTS_HPP
#define COMMON_CONSTANTS_HPP

#define DEFAULT_IPADRR          "localhost"
#define DEFAULT_PORT            "58000"

#define LOGGER_EVENT_INFO       "INFO"
#define LOGGER_EVENT_WARNING    "WARN"
#define LOGGER_EVENT_ERROR      "ERR"

#define TIMESTAMP_FORMAT        "%Y-%m-%d %H:%M:%S"

#define SOCK_BUFFER_SIZE        128
#define SERVER_RECV_TIMEOUT     10
#define TCP_BACKLOG             3
#define TCP_MAXCLIENTS          10

#define SECRET_KEY_LEN          4
#define PACKET_ID_LEN           3
#define PLAYER_ID_LEN           6

#define FNAME_MAX               24
#define FSIZE_MAX               2048

#endif