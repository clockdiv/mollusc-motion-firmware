#ifndef SERIALHANDLER_H
#define SERIALHANDLER_H

#include <Arduino.h>

class SerialHandler
{
private:
public:
    SerialHandler();
    void comma_separated_string_to_long_array(String s, long longarray[]);
    void receive();
};

#endif