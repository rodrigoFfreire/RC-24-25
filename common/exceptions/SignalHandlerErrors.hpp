#ifndef COMMON_SIGHANDLER_ERRORS_HPP
#define COMMON_SIGHANDLER_ERRORS_HPP

#include "Exceptions.hpp"

class SIGINTRegisterError : public CommonError {
private:
    static constexpr const char* errorMsg = "Failed to register SIGNINT handler! ";

public:
    SIGINTRegisterError() : CommonError(std::string(errorMsg)) {};
};

class SIGTERMRegisterError : public CommonError {
private:
    static constexpr const char* errorMsg = "Failed to register SIGTERM handler! ";

public:
    SIGTERMRegisterError() : CommonError(std::string(errorMsg)) {};
};

#endif