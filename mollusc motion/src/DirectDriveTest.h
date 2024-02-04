/* Test to drive the Steppers from the build-in Potentiometers. */

// #include <Arduino.h>
// #include "pins.h"
// #include "display.h"
// #include "averageFilter.h"

// void DirectDriveTest()
// {
//     current_millis = millis();

//     poti_a = poti_a_filtered.filter(analogRead(POTI_A));

//     if (abs(poti_a - poti_a_old) > 2 &&
//         current_millis - previous_millis > 40)
//     {
//         Serial.println(poti_a);
//         poti_a_old = poti_a;
//         previous_millis = current_millis;

//         targetPosition_1 = long(poti_a * 8); // poti: 0-8191; stepper: 0-65535

//         stepper_1.moveTo(targetPosition_1);

//         float speed_stepper_0 = stepper_1_speed_filtered.filter(fps * float(stepper_1.distanceToGo()) * 0.1f);
//         stepper_1.setSpeed(speed_stepper_0);
//     }

//     stepper_1.runSpeedToPosition();

//     if (stepper_1.distanceToGo() == 0 &&
//         stepper_2.distanceToGo() == 0 &&
//         stepper_3.distanceToGo() == 0)
//     {
//         stepper_1.setSpeed(0.0f);
//         stepper_2.setSpeed(0.0f);
//         stepper_3.setSpeed(0.0f);
//     }

//     // Enable LED_BUILTIN when a stepper is running
//     digitalWriteFast(LED_BUILTIN, stepper_1.isRunning() || stepper_2.isRunning() || stepper_3.isRunning());
// }