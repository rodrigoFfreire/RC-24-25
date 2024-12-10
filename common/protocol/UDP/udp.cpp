#include "udp.hpp"
#include <iomanip>

void StartNewGamePacket::decode(std::stringstream& packetStream) {
    UdpParser parser(packetStream);

    parser.next();
    playerID = parser.parsePlayerID();
    parser.next();
    time = parser.parseUInt();
    if (time > PLAY_TIME_MAX)
        throw InvalidPacketException();
    parser.end();
}

std::string StartNewGamePacket::encode() const {
    std::stringstream encoded_stream;
    encoded_stream << packetID << ' ';
    encoded_stream << std::setfill('0') << std::setw(PLAYER_ID_LEN) << playerID;
    encoded_stream << ' ' << time << '\n';
    return encoded_stream.str();
}

void ReplyStartGamePacket::decode(std::stringstream &packetStream) {
    UdpParser parser(packetStream);

    std::string parsed_id = parser.parsePacketID();
    if (parsed_id == UdpErrorPacket::packetID) {
        throw ErrPacketException();
    }
    if (parsed_id != ReplyStartGamePacket::packetID) {
        throw InvalidPacketException();
    }

    parser.next();

    std::string statusStr = parser.parseStatus();
    if (statusStr == "OK\n") {
        status = OK;
        packetStream.unget();
    } else if (statusStr == "NOK") {
        status = NOK;
    } else if (statusStr == "ERR") {
        status = ERR;
    } else {
        throw InvalidPacketException();
    }
    parser.end();
}

std::string ReplyStartGamePacket::encode() const {
    std::stringstream encoded_stream;
    encoded_stream << packetID << ' ';
    encoded_stream << statusToStr(status) << '\n';
    return encoded_stream.str();
}

void TryPacket::decode(std::stringstream &packetStream) {
    key.resize(SECRET_KEY_LEN, '\0');

    UdpParser parser(packetStream);
    parser.next();

    playerID = parser.parsePlayerID();
    parser.parseKey(key);
    parser.next();
    trial = parser.parseUInt();
    parser.end();
}

std::string TryPacket::encode() const {
    std::stringstream encoded_stream;
    encoded_stream << packetID << ' ';
    encoded_stream << std::setfill('0') << std::setw(PLAYER_ID_LEN) << playerID;
    for (int i = 0; i < SECRET_KEY_LEN; ++i)
        encoded_stream << ' ' << key[i];
    encoded_stream << ' ' << trial << '\n';
    return encoded_stream.str();
}

void ReplyTryPacket::decode(std::stringstream &packetStream) {
    key.resize(SECRET_KEY_LEN, '\0');

    UdpParser parser(packetStream);

    std::string parsed_id = parser.parsePacketID();
    if (parsed_id == UdpErrorPacket::packetID) {
        throw ErrPacketException();
    }
    if (parsed_id != ReplyTryPacket::packetID) {
        throw InvalidPacketException();
    }

    parser.next();
    std::string statusStr = parser.parseStatus();
    if (statusStr == "OK ") {
        status = OK;
        trial = parser.parseUInt();
        parser.next();
        blacks = parser.parseUInt();
        parser.next();
        whites = parser.parseUInt();
    } else if (statusStr == "DUP") {
        status = DUP;
    } else if (statusStr == "INV") {
        status = INV;
    } else if (statusStr == "NOK") {
        status = NOK;
    } else if (statusStr == "ENT") {
        status = ENT;
        parser.parseKey(key);
    } else if (statusStr == "ETM") {
        status = ETM;
        parser.parseKey(key);
    } else if (statusStr == "ERR") {
        status = ERR;
    } else {
        throw InvalidPacketException();
    }
    parser.end();
}

std::string ReplyTryPacket::encode() const {
    std::stringstream encoded_stream;
    encoded_stream << packetID << ' ';
    encoded_stream << statusToStr(status);
    switch (status) {
    case OK:
        encoded_stream << ' ' << trial << ' ' << blacks << ' ' << whites;
        break;
    case ENT:
    case ETM:
        for (int i = 0; i < SECRET_KEY_LEN; ++i)
            encoded_stream << ' ' << key[i];
        break;
    case DUP:
    case INV:
    case NOK:
    case ERR:
        break;
    default:
        throw PacketEncodingException();
    }

    encoded_stream << '\n';
    return encoded_stream.str();
}

void QuitPacket::decode(std::stringstream& packetStream) {
    UdpParser parser(packetStream);

    parser.next();
    playerID = parser.parsePlayerID();
    parser.end();
};

std::string QuitPacket::encode() const {
    std::stringstream encoded_stream;
    encoded_stream << packetID << ' ';
    encoded_stream << std::setfill('0') << std::setw(PLAYER_ID_LEN) << playerID << '\n';
    return encoded_stream.str();
}

void ReplyQuitPacket::decode(std::stringstream &packetStream) {
    key.resize(SECRET_KEY_LEN, '\0');

    UdpParser parser(packetStream);

    std::string parsed_id = parser.parsePacketID();
    if (parsed_id == UdpErrorPacket::packetID) {
        throw ErrPacketException();
    }
    if (parsed_id != ReplyQuitPacket::packetID) {
        throw InvalidPacketException();
    }

    parser.next();
    std::string statusStr = parser.parseStatus();
    if (statusStr == "OK ") {
        status = OK;
        packetStream.unget();
        parser.parseKey(key);
    } else if (statusStr == "NOK") {
        status = NOK;
    } else if (statusStr == "ERR") {
        status = ERR;
    } else {
        throw InvalidPacketException();
    }
    parser.end();
}

std::string ReplyQuitPacket::encode() const {
    std::stringstream encoded_stream;
    encoded_stream << packetID << ' ';
    encoded_stream << statusToStr(status);
    switch (status) {
    case OK:
        for (int i = 0; i < SECRET_KEY_LEN; ++i)
            encoded_stream << ' ' << key[i];
        break;
    case NOK:
    case ERR:
        break;
    default:
        throw PacketEncodingException();
    }

    encoded_stream << '\n';
    return encoded_stream.str();
}

void DebugPacket::decode(std::stringstream &packetStream) {
    key.resize(SECRET_KEY_LEN, '\0');

    UdpParser parser(packetStream);

    parser.next();
    playerID = parser.parsePlayerID();
    parser.next();
    time = parser.parseUInt();
    if (time > PLAY_TIME_MAX)
        throw InvalidPacketException();
    parser.parseKey(key);
    parser.end();
}

std::string DebugPacket::encode() const {
    std::stringstream encoded_stream;
    encoded_stream << packetID << ' ';
    encoded_stream << std::setfill('0') << std::setw(PLAYER_ID_LEN) << playerID;
    encoded_stream << ' ' << time;
    for (int i = 0; i < SECRET_KEY_LEN; ++i) {
        encoded_stream << ' ' << key[i];
    }
    encoded_stream << '\n';
    return encoded_stream.str();
}

void ReplyDebugPacket::decode(std::stringstream &packetStream) {
    UdpParser parser(packetStream);

    std::string parsed_id = parser.parsePacketID();
    if (parsed_id == UdpErrorPacket::packetID) {
        throw ErrPacketException();
    }
    if (parsed_id != ReplyDebugPacket::packetID) {
        throw InvalidPacketException();
    }

    parser.next();
    std::string statusStr = parser.parseStatus();
    if (statusStr == "OK\n") {
        status = OK;
        packetStream.unget();
    } else if (statusStr == "NOK") {
        status = NOK;
    } else if (statusStr == "ERR") {
        status = ERR;
    } else {
        throw InvalidPacketException();
    }
    parser.end();
}

std::string ReplyDebugPacket::encode() const {
    std::stringstream encoded_stream;
    encoded_stream << packetID << ' ';
    encoded_stream << statusToStr(status) << '\n';
    return encoded_stream.str();
}
