#include "tcp.hpp"

void ShowTrialsPacket::read(int connection_fd) {
    TcpParser parser(connection_fd);
    
    parser.next();
    playerID = parser.parsePlayerID();
    parser.end();
}

void ShowTrialsPacket::send(int connection_fd) const {
    std::ostringstream encoded_stream;
    encoded_stream << std::setfill('0') << std::setw(PLAYER_ID_LEN) << playerID << '\n';
    std::string encoded_str = encoded_stream.str();

    safe_write(connection_fd, encoded_str.c_str(), encoded_str.size());
}

void ReplyShowTrialsPacket::read(int connection_fd) {
    TcpParser parser(connection_fd);

    std::string parsed_id = parser.parsePacketID();
    if (parsed_id == TcpErrorPacket::packetID) {
        throw ErrPacketException();
    }
    if (parsed_id != ReplyShowTrialsPacket::packetID) {
        throw InvalidPacketException();
    }

    parser.next();
    std::string statusStr = parser.parseStatus();
    if (statusStr == "NOK") {
        status = NOK;
    } else {
        if (statusStr == "ACT")
            status = ACT;
        else if (statusStr == "FIN")
            status = FIN;
        else
            throw InvalidPacketException();

        fname = parser.parseFileName();
        fsize = parser.parseFileSize();
        fdata = parser.parseFile(fsize);
    }
    parser.end();
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

void ReplyShowScoreboardPacket::send(int connection_fd) const
{
    std::ostringstream encoded_stream;
    encoded_stream << packetID << ' ' << statusToStr(status) << '\n';
    std::string encoded_str = encoded_stream.str();

    safe_write(connection_fd, encoded_str.c_str(), encoded_str.size());
}
