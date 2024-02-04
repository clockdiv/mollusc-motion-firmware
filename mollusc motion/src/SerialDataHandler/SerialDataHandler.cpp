#include "SerialDataHandler.h"

SerialDataHandler::SerialDataHandler()
{
}

void SerialDataHandler::comma_separated_string_to_long_array(String s, long longarray[])
{
    String remaining_string = s;
    int index_comma = -1;
    int i = 0;
    while ((index_comma = remaining_string.indexOf(',')) > 0)
    {
        String current_value;
        if (index_comma > 0)
        {
            current_value = remaining_string.substring(0, index_comma);
            remaining_string = remaining_string.substring(index_comma + 1, remaining_string.length());
        }
        else
        {
            current_value = remaining_string;
        }
        longarray[i] = current_value.toInt();
        i++;
    }
    longarray[i] = remaining_string.toInt();
}

bool SerialDataHandler::receiveAsCSV()
{
    if (Serial.available() > 0)
    {
        // current_millis_serial_received = millis();
        String incomingString = Serial.readStringUntil('\n');
        if (incomingString == "MANUAL")
        {
            serialData.command = STATE_CHANGE;
            strcpy(serialData.stateAsString, "MANUAL");
        }
        else if (incomingString == "RUNNING")
        {
            serialData.command = STATE_CHANGE;
            strcpy(serialData.stateAsString, "RUNNING");
        }
        else
        {
            serialData.command = SerialCommand::POSITION_DATA;
            long targetPositions[14];
            comma_separated_string_to_long_array(incomingString, targetPositions);
            // index 0-2 are for the Stepper Motors, 3-13 are for the Servos
            memcpy(serialData.targetPositionsSteppers, targetPositions, 3);
            memcpy(serialData.targetPositionsServos, targetPositions + 3, 11);
        }
        return true;
    }
    return false;
}
