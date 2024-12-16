#include "tcp.hpp"
#include <iostream>

void ShowTrialsPacket::read(int connection_fd) {
    playerID.resize(PLID_LEN, '\0');

    TcpParser parser(connection_fd);
    
    parser.next();
    playerID = parser.parsePlayerID();
    parser.end();
}

std::string ShowTrialsPacket::send(int connection_fd) const {
    std::ostringstream encoded_stream;
    encoded_stream << packetID << ' ' << playerID << '\n';
    std::string encoded_str = encoded_stream.str();

    safe_write(connection_fd, encoded_str.c_str(), encoded_str.size());

    return encoded_str;
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

        parser.next();
        fname = parser.parseFileName();
        fsize = parser.parseFileSize();
        fdata = parser.parseFile(fsize);
    }
    parser.end();
}

std::string ReplyShowTrialsPacket::send(int connection_fd) const {
    std::ostringstream encoded_stream;
    encoded_stream << packetID << ' ' << statusToStr(status);
    switch (status) {
    case ReplyShowTrialsPacket::NOK:
        break;
    case ReplyShowTrialsPacket::FIN:
    case ReplyShowTrialsPacket::ACT:
        encoded_stream << ' ' << fname << ' ' << fsize << ' ' << fdata;
        break;
    default:
        throw PacketEncodingException();
    }

    encoded_stream << '\n';
    std::string encoded_str = encoded_stream.str();

    safe_write(connection_fd, encoded_str.c_str(), encoded_str.size());

    return encoded_str;
}

void ShowScoreboardPacket::read(int connection_fd) {
    TcpParser parser(connection_fd);

    parser.end();
}

std::string ShowScoreboardPacket::send(int connection_fd) const {
    std::ostringstream encoded_stream;
    encoded_stream << packetID << '\n';
    std::string encoded_str = encoded_stream.str();

    safe_write(connection_fd, encoded_str.c_str(), encoded_str.size());

    return encoded_str;
}

void ReplyShowScoreboardPacket::read(int connection_fd) {
    TcpParser parser(connection_fd);

    std::string parsed_id = parser.parsePacketID();
    if (parsed_id == TcpErrorPacket::packetID) {
        throw ErrPacketException();
    }
    if (parsed_id != ReplyShowScoreboardPacket::packetID) {
        throw InvalidPacketException();
    }

    parser.next();
    std::string statusStr = parser.parseStatus();
    if (statusStr == "EMP") {
        parser.checkNextChar('T');
        parser.checkNextChar('Y');
        status = EMPTY;
    } else if (statusStr == "OK ") {
        status = OK;

        fname = parser.parseFileName();
        fsize = parser.parseFileSize();
        fdata = parser.parseFile(fsize);
    } else {
        throw InvalidPacketException();
    }
    parser.end();
}

std::string ReplyShowScoreboardPacket::send(int connection_fd) const {
    std::ostringstream encoded_stream;
    encoded_stream << packetID << ' ' << statusToStr(status);
    switch (status) {
    case ReplyShowScoreboardPacket::EMPTY:
        break;
    case ReplyShowScoreboardPacket::OK:
        encoded_stream << ' ' << fname << ' ' << fsize << ' ' << fdata;
        break;
    default:
        throw PacketEncodingException();
    }

    encoded_stream << '\n';
    std::string encoded_str = encoded_stream.str();

    safe_write(connection_fd, encoded_str.c_str(), encoded_str.size());

    return encoded_str;
}
