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

class UncontextualizedGameException : public CommonException {
private:
    const std::string errorMsg = "This player has no ongoing game!";

public:
    UncontextualizedGameException() : CommonException(errorMsg) {};
};

class TimedoutGameException : public CommonException {
private:
    const std::string errorMsg = "This player has exceeded the maximum play time!";

public:
    TimedoutGameException() : CommonException(errorMsg) {};
};

class InvalidTrialException : public CommonException {
private:
    const std::string errorMsg = "This player has attempted an invalid trial!";

public:
    InvalidTrialException() : CommonException(errorMsg) {};
};

class DuplicateTrialException : public CommonException {
private:
    const std::string errorMsg = "This player has attempted a duplicate trial!";

public:
    DuplicateTrialException() : CommonException(errorMsg) {};
};

class ExceededMaxTrialsException : public CommonException {
private:
    const std::string errorMsg = "This player has used all of their attempts!";

public:
    ExceededMaxTrialsException() : CommonException(errorMsg) {};
};

#endif