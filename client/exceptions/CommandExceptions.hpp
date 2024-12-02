#ifndef CLIENT_COMMAND_EXCEPTIONS_HPP
#define CLIENT_COMMAND_EXCEPTIONS_HPP

#include "../../common/exceptions/Exceptions.hpp"

class UnexpectedCommandException : public CommonException {
private:
    const std::string errorMsg = "This command does not exist!";

public:
    UnexpectedCommandException() : CommonException(errorMsg) {};
};

class BadCommandException : public CommonException {
private:
    const std::string errorMsg = "The server could not process this command!";

public:
    BadCommandException() : CommonException(errorMsg) {};
};

class InvalidPlayerIDException : public CommonException {
private:
    const std::string errorMsg = "Invalid player ID! (0-999999)!";

public:
    InvalidPlayerIDException() : CommonException(errorMsg) {};
};

class InvalidPlayTimeException : public CommonException {
private:
    const std::string errorMsg = "Invalid play time! (1-600)";

public:
    InvalidPlayTimeException() : CommonException(errorMsg) {};
};

class PendingGameException : public CommonException {
private:
    const std::string errorMsg = "You already have a pending game!";

public:
    PendingGameException() : CommonException(errorMsg) {};
};

class DupAttemptException : public CommonException {
private:
    const std::string errorMsg = "This guess has been tried before!";

public:
    DupAttemptException() : CommonException(errorMsg) {};
};

class InvalidAttemptException : public CommonException {
private:
    const std::string errorMsg = "Invalid attempt! (trial number)";

public:
    InvalidAttemptException() : CommonException(errorMsg) {};
};

class InvalidKeyException : public CommonException {
private:
    const std::string errorMsg = "Invalid key! Format: C1 C2 C3 C4. Run ? to get more information";

public:
    InvalidKeyException() : CommonException(errorMsg) {};
};

class UncontextualizedException : public CommonException {
private:
    const std::string errorMsg = "You are not currently playing a game!";

public:
    UncontextualizedException() : CommonException(errorMsg) {};
};

#endif