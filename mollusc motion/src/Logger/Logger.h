#ifndef LOGGER_H
#define LOGGER_H

#include <Arduino.h>
#include "../SDCardHelpers/SDCardHelpers.h"

class Logger
{
private:
    enum logtype
    {
        INFO,
        WARNING,
        ERROR
    };

    void Process_log_message(String message, int type);
    void Log_to_Serial(String message);
    bool Log_to_SD(String message);
    bool Log_to_Server(String message);

public:
    void Info(String message);
    void Warning(String message);
    void Error(String message);
};

#endif