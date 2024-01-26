#include "Logger.h"

void Logger::Info(String message)
{
    Process_log_message(message, INFO);
}

void Logger::Warning(String message)
{
    Process_log_message(message, WARNING);
}

void Logger::Error(String message)
{
    Process_log_message(message, ERROR);
}

void Logger::Process_log_message(String message, int type)
{
    String log_msg = "";

    // log_msg += "01.01.2024 12:00 ";
    log_msg += String(millis()) + " ";

    switch (type)
    {
    case INFO:
        log_msg += "INFO    ";
        break;
    case WARNING:
        log_msg += "WARNING ";
        break;
    case ERROR:
        log_msg += "ERROR   ";
        break;
    default:
        break;
    }

    log_msg += message;

    Log_to_SD(log_msg);
    Log_to_Serial(log_msg);
    Log_to_Server(log_msg);
}

void Logger::Log_to_Serial(String message)
{
    Serial.println(message);
}

bool Logger::Log_to_SD(String message)
{
    File dataFile = SD.open("datalog.txt", FILE_WRITE);

    if (!dataFile)
        return false;

    dataFile.println(message);
    dataFile.close();
    return true;
}

bool Logger::Log_to_Server(String message)
{
    return false;
}
