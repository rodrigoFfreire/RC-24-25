#ifndef COMMON_PROTOCOL_TCP_PACKETS_HPP
#define COMMON_PROTOCOL_TCP_PACKETS_HPP

#include "Packet.hpp"

class ShowTrialsPacket : public Packet {
public:
    static constexpr const char* packetID = "STR";
    uint32_t playerID;

    void decode(std::stringstream& packetStream);
    std::string encode() const override {return "";};
};

class ReplyShowTrialsPacket : public Packet {
public:
    static constexpr const char* packetID = "RST";
    enum Status { ACT, FIN, NOK };
    Status status;
    std::string fname;
    std::string fdata;
    uint32_t fsize;

    void decode(std::stringstream& packetStream) override {};
    std::string encode() const override {return "";};
};

class ShowScoreboardPacket : public Packet {
public:
    static constexpr const char* packetID = "SSB";

    void decode(std::stringstream& packetStream) override {};
    std::string encode() const override {return "";};
};

class ReplyShowScoreboardPacket : public Packet {
public:
    static constexpr const char* packetID = "STR";
    enum Status { EMPTY, OK };
    Status status;
    std::string fname;
    std::string fdata;
    uint32_t fsize;
    
    void decode(std::stringstream& packetStream) override {};
    std::string encode() const override {return "";};
};

#endif