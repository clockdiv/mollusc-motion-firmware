#ifndef TIMEWRAPPER_H
#define TIMEWRAPPER_H

#include <Arduino.h>
#include <TimeLib.h>

class TimeWrapper
{
private:
    static String formatNumber(int number);

public:
    TimeWrapper();
    static String currentTimeAsString();
    static String dateFormattedAsFilePrefix();
    static bool checkStatus();
    static const void setCurrentTime(unsigned long pctime);
};

#endif