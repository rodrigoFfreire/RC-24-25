#ifndef SERVER_GAME_EXCEPTIONS_HPP
#define SERVER_GAME_EXCEPTIONS_HPP

#include "../../common/exceptions/Exceptions.hpp"

class OngoingGameException : public CommonException {
private:
    const std::string errorMsg = "This player has an ongoing game!";

public:
    OngoingGameException() : CommonException(errorMsg) {};
};

class InvalidGameModeException : public CommonException {
private:
    const std::string errorMsg = "That gamemode is not recognized";

public:
    InvalidGameModeException() : CommonException(errorMsg) {};
};

#endif