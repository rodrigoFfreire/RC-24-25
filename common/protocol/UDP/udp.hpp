#ifndef COMMON_PROTOCOL_UDP_PACKETS_HPP
#define COMMON_PROTOCOL_UDP_PACKETS_HPP

#include "Parser.hpp"

class UdpPacket {
public:
    virtual void decode(std::stringstream& packetStream) = 0;
    virtual std::string encode() const = 0;
};

class StartNewGamePacket : public UdpPacket {
public:
    static constexpr const char* packetID = "SNG";
    unsigned int time;
    unsigned int playerID;

    void decode(std::stringstream& packetStream) override;
    std::string encode() const override;
};

class ReplyStartGamePacket : public UdpPacket {
public:
    static constexpr const char* packetID = "RSG";
    enum Status { OK, NOK, ERR };
    Status status;

    std::string statusToStr(Status status) const {
        switch (status) {
        case OK:
            return "OK";
        case NOK:
            return "NOK";
        case ERR:
            return "ERR";
        default:
            throw PacketEncodingException();
        }
    };

    void decode(std::stringstream& packetStream) override;
    std::string encode() const override;
};

class TryPacket : public UdpPacket {
public:
    static constexpr const char* packetID = "TRY";
    unsigned int playerID;
    unsigned int trial;
    char key[SECRET_KEY_LEN];

    void decode(std::stringstream& packetStream) override;
    std::string encode() const override;
};

class ReplyTryPacket : public UdpPacket {
public:
    static constexpr const char* packetID = "RTR";
    enum Status { OK, DUP, INV, NOK, ENT, ETM, ERR };
    Status status;
    unsigned int trial;
    unsigned int whites;
    unsigned int blacks;
    char key[SECRET_KEY_LEN];

    std::string statusToStr(Status status) const {
        switch (status) {
        case OK:
            return "OK";
        case DUP:
            return "DUP";
        case INV:
            return "INV";
        case NOK:
            return "NOK";
        case ENT:
            return "ENT";
        case ETM:
            return "ETM";
        case ERR:
            return "ERR";
        default:
            throw PacketEncodingException();
        }
    };

    void decode(std::stringstream& packetStream) override;
    std::string encode() const override;
};

class QuitPacket : public UdpPacket {
public:
    static constexpr const char* packetID = "QUT";
    unsigned int playerID;

    void decode(std::stringstream& packetStream) override;
    std::string encode() const override;
};

class ReplyQuitPacket : public UdpPacket {
public:
    static constexpr const char* packetID = "RQT";
    enum Status { OK, NOK, ERR };
    Status status;
    char key[SECRET_KEY_LEN];

    std::string statusToStr(Status status) const {
        switch (status) {
        case OK:
            return "OK";
        case NOK:
            return "NOK";
        case ERR:
            return "ERR";
        default:
            throw PacketEncodingException();
        }
    };

    void decode(std::stringstream& packetStream) override;
    std::string encode() const override;
};

class DebugPacket : public UdpPacket {
public:
    static constexpr const char* packetID = "DBG";
    unsigned int playerID;
    unsigned int time;
    char key[SECRET_KEY_LEN];

    void decode(std::stringstream& packetStream) override;
    std::string encode() const override;
};

class ReplyDebugPacket : public UdpPacket {
public:
    static constexpr const char* packetID = "RDB";
    enum Status { OK, NOK, ERR };
    Status status;

    std::string statusToStr(Status status) const {
        switch (status) {
        case OK:
            return "OK";
        case NOK:
            return "NOK";
        case ERR:
            return "ERR";
        default:
            throw PacketEncodingException();
        }
    };
    
    void decode(std::stringstream& packetStream) override;
    std::string encode() const override;
};

class UdpErrorPacket : public UdpPacket {
public:
    static constexpr const char* packetID = "ERR";

    void decode(std::stringstream& packetStream) override {(void)packetStream;};
    std::string encode() const override {return "ERR\n";};
};

#endif