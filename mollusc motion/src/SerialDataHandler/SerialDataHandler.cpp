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
        // Serial.println(incomingString);
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
        else if (incomingString == "IDLE")
        {
            serialData.command = STATE_CHANGE;
            strcpy(serialData.stateAsString, "IDLE");
        }
        else if (incomingString == "HOMINGA")
        {
            serialData.command = STATE_CHANGE;
            strcpy(serialData.stateAsString, "HOMING_A");
        }
        else if (incomingString.charAt(0) == 'T')
        {
            serialData.command = SET_TIME;
            // Serial.println("XXXXXXXXXX RECEIVED CHAR");
            // Serial.println(incomingString);
            incomingString = incomingString.substring(1);
            // Serial.println(incomingString);
            // char pctime[16];
            // strcpy(pctime, incomingString.c_str());
            serialData.pctime = incomingString.toInt();
            // Serial.println(serialData.pctime);
            // Serial.println("XXXXXXXXXX RECEIVED CHAR END");
        }
        else
        {
            serialData.command = SerialCommand::POSITION_DATA;
            // long targetPositions[14];
            long targetPositions[4];
            comma_separated_string_to_long_array(incomingString, targetPositions);

            // // index 0-2 are for the Stepper Motors, 3-13 are for the Servos
            // memcpy(serialData.targetPositionsSteppers, targetPositions, 3);
            // memcpy(serialData.targetPositionsServos, targetPositions + 3, 11);

            // index 0-3 are for the Stepper Motors
            memcpy(serialData.targetPositionsSteppers, targetPositions, sizeof(targetPositions));
        }
        return true;
    }
    return false;
}
