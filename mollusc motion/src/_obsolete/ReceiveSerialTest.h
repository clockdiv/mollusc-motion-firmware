#include <Arduino.h>
#include "pins.h"
#include "state.h"
// #include "averageFilter.h"
// #include "DynamixelTest.h"

// void debug_to_serial_full(String preText = "")
// {
//     if (preText != "")
//         Serial.println(preText);

//     // TODO enable again!
//     // Serial.println("Current Pos: " + String(stepper_1.currentPosition()) +
//     //                //    "\t Target: " + String(targetPosition_1) +
//     //                "\t DistanceToGo: " + String(stepper_1.distanceToGo()) +
//     //                "\t Speed: " + String(stepper_1.speed()) +
//     //                "\t Fps: " + String(fps));
// }

// void debug_to_serial_minimal()
// {
//     // TODO enable again!
//     // Serial.println(String(stepper_1.currentPosition()) +
//     //                //    "," + String(targetPosition_1) +
//     //                "," + String(stepper_1.distanceToGo()) +
//     //                "," + String(stepper_1.speed()) +
//     //                "," + String(fps));
// }

// void comma_separated_string_to_long_array(String s, long longarray[])
// {
//     String remaining_string = s;
//     int index_comma = -1;
//     int i = 0;
//     while ((index_comma = remaining_string.indexOf(',')) > 0)
//     {
//         String current_value;
//         if (index_comma > 0)
//         {
//             current_value = remaining_string.substring(0, index_comma);
//             remaining_string = remaining_string.substring(index_comma + 1, remaining_string.length());
//         }
//         else
//         {
//             current_value = remaining_string;
//         }
//         longarray[i] = current_value.toInt();
//         i++;
//     }
//     longarray[i] = remaining_string.toInt();
// }

// void ReceiveSerial()
// {
//     if (Serial.available() > 0)
//     {
//         // current_millis_serial_received = millis();

//         // Read the target Positions as CSV-String from Serial
//         String incomingString = Serial.readStringUntil('\n');

//         if (incomingString == "MANUAL")
//         {
//             set_state(MANUAL);
//         }
//         else if (incomingString == "RUNNING")
//         {
//             set_state(RUNNING);
//         }
//         else
//         {
//             long targetPositions[14];
//             comma_separated_string_to_long_array(incomingString, targetPositions);

//             // TODO enable again!
//             // setNewStepperPositions();
//             // setNewDynamixelPositions();

//             if (state == RUNNING)
//             {
//                 // TODO enable again!
//                 // float speed_stepper_1 = stepper_1_speed_filtered.filter(fps * float(stepper_1.distanceToGo()) * 0.1f);
//                 // stepper_1.setSpeed(speed_stepper_1);
//                 // TODO enable again!
//                 // float speed_stepper_2 = stepper_2_speed_filtered.filter(fps * float(stepper_2.distanceToGo()) * 0.1f);
//                 // stepper_2.setSpeed(speed_stepper_2);
//                 // TODO enable again!
//                 // float speed_stepper_3 = stepper_3_speed_filtered.filter(fps * float(stepper_3.distanceToGo()) * 0.1f);
//                 // stepper_3.setSpeed(speed_stepper_3);
//             }
//         }
//     }
// }
