#ifndef COMMON_PROTOCOL_UDP_PACKETS_HPP
#define COMMON_PROTOCOL_UDP_PACKETS_HPP

#include "Packet.hpp"

class StartNewGamePacket : public Packet {
private:
    static constexpr const char* packetID = "SNG";
    u_int16_t time;
    u_int32_t playerID;

public:
    void decode(std::stringstream& packetStream);
    std::string encode() const;
};

class ReplyStartGamePacket : public Packet {
private:
    static constexpr const char* packetID = "RSG";
    enum Status { OK, NOK, ERR };
    Status status;

public:
    void decode(std::stringstream& packetStream);
    void encode(const std::stringstream& packetStream) const;
};

class TryPacket : public Packet {
private:
    static constexpr const char* packetID = "TRY";
    u_int32_t playerID;
    u_int32_t trial;
    char key[SECRET_KEY_LEN];

public:
    void decode(std::stringstream& packetStream);
    void encode(const std::stringstream& packetStream) const;
};

class ReplyTryPacket : public Packet {
private:
    static constexpr const char* packetID = "RTR";
    enum Status { OK, DUP, INV, NOK, ENT, ETM, ERR };
    u_int32_t trial;
    u_int8_t whites;
    u_int8_t blacks;
    char key[SECRET_KEY_LEN];

public:
    void decode(std::stringstream& packetStream);
    void encode(const std::stringstream& packetStream) const;
};

class QuitPacket : public Packet {
private:
    static constexpr const char* packetID = "QUT";
    u_int32_t playerID;

public:
    void decode(std::stringstream& packetStream);
    void encode(const std::stringstream& packetStream) const;
};

class ReplyQuitPacket : public Packet {
private:
    static constexpr const char* packetID = "RQT";
    enum Status { OK, NOK, ERR };
    Status status;
    char key[SECRET_KEY_LEN];

public:
    void decode(std::stringstream& packetStream);
    void encode(const std::stringstream& packetStream) const;
};

class DebugPacket : public Packet {
private:
    static constexpr const char* packetID = "DBG";
    u_int32_t playerID;
    u_int16_t time;
    char key[SECRET_KEY_LEN];

public:
    void decode(std::stringstream& packetStream);
    void encode(const std::stringstream& packetStream) const;
};

class ReplyDebugPacket : public Packet {
private:
    static constexpr const char* packetID = "RBD";
    enum Status { OK, NOK, ERR };
    Status status;

public:
    void decode(std::stringstream& packetStream);
    void encode(const std::stringstream& packetStream) const;
};

#endif