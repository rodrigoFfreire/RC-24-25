#include "tcp.hpp"

void ShowTrialsPacket::read(int connection_fd) {
    TcpParser parser(connection_fd);
    
    parser.next();
    playerID = parser.parsePlayerID();
    parser.end();
}

void ShowTrialsPacket::send(int connection_fd) const {
    std::ostringstream encoded_stream;
    encoded_stream << packetID << ' ' << playerID << '\n';
    std::string encoded_str = encoded_stream.str();

    safe_write(connection_fd, encoded_str.c_str(), encoded_str.size());
}

void ReplyShowTrialsPacket::send(int connection_fd) const {
    std::ostringstream encoded_stream;
    encoded_stream << packetID << ' ' << statusToStr(status) << '\n';
    std::string encoded_str = encoded_stream.str();

    safe_write(connection_fd, encoded_str.c_str(), encoded_str.size());
}

void ShowScoreboardPacket::read(int connection_fd) {
    TcpParser parser(connection_fd);

    parser.end();
}

void ShowScoreboardPacket::send(int connection_fd) const {
    std::string encoded_str(packetID + '\n');

    safe_write(connection_fd, encoded_str.c_str(), encoded_str.size());
}

void ReplyShowScoreboardPacket::send(int connection_fd) const {
    std::ostringstream encoded_stream;
    encoded_stream << packetID << ' ' << statusToStr(status) << '\n';
    std::string encoded_str = encoded_stream.str();

    safe_write(connection_fd, encoded_str.c_str(), encoded_str.size());
}
