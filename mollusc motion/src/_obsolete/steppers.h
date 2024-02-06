#ifndef INITSTEPPERS_H
#define INITSTEPPERS_H

#include <Arduino.h>
// #include "AccelStepper/AccelStepper.h"
#include <AccelStepper.h>
#include "pins.h"

// void zeroPositions()
// {
//     stepper_1.setCurrentPosition(0);
//     stepper_2.setCurrentPosition(0);
//     stepper_3.setCurrentPosition(0);
// }

// void setNewStepperPositions()
// {
//     // Serial.printf("Stepper 1 target pos: %d\n", targetPositions[0]);

//     stepper_1.moveTo(targetPositions[0]);
//     stepper_2.moveTo(targetPositions[1]);
//     stepper_3.moveTo(targetPositions[2]);

//     // Serial.printf("Stepper 1 target pos: %d\n", stepper_1.targetPosition());
// }

// void initIdle()
// {
//     Serial.println("initIdle()");
//     stepper_1.setSpeed(0);
//     stepper_2.setSpeed(0);
//     stepper_3.setSpeed(0);

//     Serial.println(stepper_1.distanceToGo());
//     Serial.println(stepper_2.distanceToGo());
//     Serial.println(stepper_3.distanceToGo());
//     Serial.println(stepper_1.isRunning());
//     Serial.println(stepper_2.isRunning());
//     Serial.println(stepper_3.isRunning());
// }

// void initRunning()
// {
//     Serial.println("initRunning()");

//     // const float acceleration = 10000;

//     stepper_1.setMaxSpeed(stepper_1_maxSpeed);
//     stepper_2.setMaxSpeed(stepper_2_maxSpeed);
//     stepper_3.setMaxSpeed(stepper_3_maxSpeed);

//     // stepper_1.setAcceleration(acceleration);
//     // stepper_2.setAcceleration(acceleration);
//     // stepper_3.setAcceleration(acceleration);
// }

// void initManual()
// {
//     // this is called when the user 'jumps' on the
//     // timeline in Blender. The motors are moved with
//     // acceleration to their target positions.
//     Serial.println("initManual()");

//     // all values for MaxSpeed and Acceleration tested
//     // Stepper 1 runs with 800 MicroSteps
//     // Stepper 2 runs with 1600 MicroSteps
//     // Stepper 3 runs with 1600 MicroSteps
//     stepper_1.setMaxSpeed(stepper_1_maxSpeed / 2); // half maximum speed,
//     stepper_2.setMaxSpeed(stepper_2_maxSpeed / 2); // we don't need to stress
//     stepper_3.setMaxSpeed(stepper_3_maxSpeed / 2); // our mechanics more than needed

//     stepper_1.setAcceleration(60000);
//     stepper_2.setAcceleration(40000);
//     stepper_3.setAcceleration(60000);
// }

// void initHoming_A()
// {
//     Serial.println("initHoming_A()");

//     // const float acceleration = 10000;

//     stepper_1.setMaxSpeed(2000);
//     stepper_2.setMaxSpeed(2000);
//     stepper_3.setMaxSpeed(2000);

//     // stepper_1.setAcceleration(acceleration);
//     // stepper_2.setAcceleration(acceleration);
//     // stepper_3.setAcceleration(acceleration);

//     stepper_1.setSpeed(-1600);
//     stepper_2.setSpeed(-1600);
//     stepper_3.setSpeed(-1600);
// }

// bool driveHoming_A()
// {
//     bool e0 = end_1.isPressed();
//     bool e1 = end_2.isPressed();
//     bool e2 = end_3.isPressed();
//     if (!e0)
//     {
//         stepper_1.setSpeed(-1600);
//         stepper_1.runSpeed();
//     }
//     if (!e1)
//     {
//         stepper_2.setSpeed(-1600);
//         stepper_2.runSpeed();
//     }
//     if (!e2)
//     {
//         stepper_3.setSpeed(-1600);
//         stepper_3.runSpeed();
//     }

//     return e0 && e1 && e2;
// }

// void initHoming_B()
// {
//     Serial.println("initHoming_B");
//     const float acceleration = 1000;

//     stepper_1.setMaxSpeed(1600);
//     stepper_2.setMaxSpeed(3200);
//     stepper_3.setMaxSpeed(3200);

//     stepper_1.setAcceleration(acceleration);
//     stepper_2.setAcceleration(acceleration);
//     stepper_3.setAcceleration(acceleration);

//     stepper_1.move(800);  // here: 5mm
//     stepper_2.move(1600); // here: 5mm
//     stepper_3.move(800);  // here: 2mm

//     // stepper_1.setSpeed(800);
//     // stepper_2.setSpeed(500);
//     // stepper_3.setSpeed(500);
// }

// bool driveHoming_B()
// {
//     stepper_1.run();
//     stepper_2.run();
//     stepper_3.run();

//     bool done = stepper_1.distanceToGo() == 0 &&
//                 stepper_2.distanceToGo() == 0 &&
//                 stepper_3.distanceToGo() == 0;

//     return done;
// }
#endif