#ifndef SERIALHANDLER_H
#define SERIALHANDLER_H

#include <Arduino.h>

enum SerialCommand
{
    STATE_CHANGE,
    POSITION_DATA,
    SET_TIME
};

class SerialDataHandler
{
private:
    struct SerialData
    {
        SerialCommand command;
        char stateAsString[16];
        long targetPositionsSteppers[4];
        long targetPositionsServos[16];
        unsigned long pctime;
    };
    // struct IDPosPair
    // {
    //     uint8_t id;
    //     long position;
    // };
    // struct FrameData
    // {
    //     uint8_t stepperCount;
    //     uint8_t servoCount;
    //     IDPosPair stepperTargets[4];
    //     IDPosPair servoTargets[16];
    // };

public:
    SerialDataHandler();
    void comma_separated_string_to_long_array(String s, long longarray[]);
    bool receiveAsCSV();

    // FrameData frameData;

    SerialData serialData; // holds the data that comes from the serial port
};

#endif