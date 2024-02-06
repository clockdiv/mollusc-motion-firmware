#include "TimeWrapper.h"

TimeWrapper::TimeWrapper()
{
}

String TimeWrapper::formatNumber(int number)
{
    String n;
    if (number > 9)
    {
        n = String(number);
    }
    else
    {
        n = "0" + String(number);
    }
    return n;
}

String TimeWrapper::currentTimeAsString()
{
    if (timeStatus() == timeSet)
    {
        return String(year()) + "/" + formatNumber(month()) + "/" + formatNumber(day()) + " " + formatNumber(hour()) + ":" + formatNumber(minute()) + ":" + formatNumber(second());
    }
    else
    {
        return F("Time Not Set");
    }
}
String TimeWrapper::dateFormattedAsFilePrefix()
{
    if (timeStatus() == timeSet)
    {
        return String(year()) + formatNumber(month()) + formatNumber(day());
    }
    else
    {
        return "";
    }
}

bool TimeWrapper::checkStatus()
{
    if (timeStatus() == timeSet)
    {
        return true;
    }
    else
    {
        return false;
    }

    /*
    switch (timeStatus())
    {
    case timeSet:
        return "Status: Time set";
        break;
    case timeNotSet:
        return "Status: Time not set";
        break;
    case timeNeedsSync:
        // I don't use a automatic sync function at the moment... return "Status: Time needs sync";
        break;
    default:
        return false;
    }
    return "Status: unknown";
    */
}

const void TimeWrapper::setCurrentTime(unsigned long pctime)
{
    const unsigned long DEFAULT_TIME = 1357041600; // Jan 1 2013
    if (pctime >= DEFAULT_TIME)
    {
        setTime(pctime);
    }
    else
    {
        setTime(DEFAULT_TIME);
        Serial.println("Time not valid. Set default time.");
    }
}
