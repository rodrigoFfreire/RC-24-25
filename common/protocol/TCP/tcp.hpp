#ifndef COMMON_PROTOCOL_TCP_PACKETS_HPP
#define COMMON_PROTOCOL_TCP_PACKETS_HPP

#include <iomanip>
#include <string>

#include "Parser.hpp"

class TcpPacket {
 public:
  virtual void read(int connection_fd) = 0;
  virtual std::string send(int connection_fd) const = 0;
  virtual ~TcpPacket() = default;
};

class ShowTrialsPacket : public TcpPacket {
 public:
  static constexpr const char* packetID = "STR";
  std::string playerID;

  void read(int connection_fd) override;
  std::string send(int connection_fd) const override;
};

class ReplyShowTrialsPacket : public TcpPacket {
 public:
  static constexpr const char* packetID = "RST";
  enum Status { ACT, FIN, NOK };
  Status status;
  std::string fname;
  std::string fdata;
  unsigned short fsize;

  std::string statusToStr(Status status) const {
    switch (status) {
      case ACT:
        return "ACT";
      case FIN:
        return "FIN";
      case NOK:
        return "NOK";
      default:
        throw PacketEncodingException();
    }
  };

  void read(int connection_fd) override;
  std::string send(int connection_fd) const override;
};

class ShowScoreboardPacket : public TcpPacket {
 public:
  static constexpr const char* packetID = "SSB";

  void read(int connection_fd) override;
  std::string send(int connection_fd) const override;
};

class ReplyShowScoreboardPacket : public TcpPacket {
 public:
  static constexpr const char* packetID = "RSS";
  enum Status { EMPTY, OK };
  Status status;
  std::string fname;
  std::string fdata;
  unsigned short fsize;

  std::string statusToStr(Status status) const {
    switch (status) {
      case EMPTY:
        return "EMPTY";
      case OK:
        return "OK";
      default:
        throw PacketEncodingException();
    }
  };

  void read(int connection_fd) override;
  std::string send(int connection_fd) const override;
};

class TcpErrorPacket : public TcpPacket {
 public:
  static constexpr const char* packetID = "ERR";

  void read(int connection_fd) override { (void)connection_fd; };
  std::string send(int connection_fd) const override {
    safe_write(connection_fd, "ERR\n", 4);
    return "ERR\n";
  };
};

#endif