#ifndef PINS_H
#define PINS_H

#include <Arduino.h>
#include "AccelStepper/AccelStepper.h"
// #include "Dynamixel2Arduino/Dynamixel2Arduino.h"
// #include "LiquidCrystal_I2C/LiquidCrystal_I2C.h"
#include "Bounce2/Bounce2.h"
#include "averageFilter.h"

#include "Logger/Logger.h"
#include "SDCardHelpers/SDCardHelpers.h"
#include "Dynamixel/Dynamixel.h"
#include "NeoPixels/NeoPixels.h"

// Pins for Board Version 1.0
// #define STEPPER_1_DIR 2
// #define STEPPER_1_PULSE 3
// #define STEPPER_1_END 4
// #define STEPPER_2_DIR 5
// #define STEPPER_2_PULSE 6
// #define STEPPER_2_END 7
// #define STEPPER_3_DIR 21
// #define STEPPER_3_PULSE 20
// #define STEPPER_3_END 22
// #define STEPPER_ENABLE 23
// #define BTN_A 14
// #define POTI_A 15
// #define BTN_B 17
// #define POTI_B 16

// Pins for Board Version 1.2
// Stepper Motors:
#define STEPPER_3_DIR 0
#define STEPPER_3_PULSE 1
#define STEPPER_2_DIR 2
#define STEPPER_2_PULSE 3
#define STEPPER_1_DIR 4
#define STEPPER_1_PULSE 5
#define STEPPER_3_END 6
#define STEPPER_2_END 7
#define STEPPER_1_END 8
#define STEPPER_ENABLE 9

// Buttons + Potis:
#define BTN_A 14
#define POTI_A 15
#define POTI_B 16
#define BTN_B 17

// Display
// LiquidCrystal_I2C lcd(0x27, 20, 4); // set the LCD address to 0x27 for a 16 chars and 2 line display


// Stepper Motors
AccelStepper stepper_1(AccelStepper::DRIVER, STEPPER_1_PULSE, STEPPER_1_DIR);
AccelStepper stepper_2(AccelStepper::DRIVER, STEPPER_2_PULSE, STEPPER_2_DIR);
AccelStepper stepper_3(AccelStepper::DRIVER, STEPPER_3_PULSE, STEPPER_3_DIR);

averageFilter stepper_1_speed_filtered;
averageFilter stepper_2_speed_filtered;
averageFilter stepper_3_speed_filtered;

const float maxSpeed_Offset = 10000; // turn all a bit slower...
// we figured out that the values below do work with our robot,
// but it's super fast and stresses the mechanics quite a lot
const float stepper_1_maxSpeed = 30000 - maxSpeed_Offset;
const float stepper_2_maxSpeed = 20000 - maxSpeed_Offset;
const float stepper_3_maxSpeed = 20000 - maxSpeed_Offset;

unsigned int minPulseWidth = 3;

// temporary for test

// Buttons
Bounce2::Button btn_a = Bounce2::Button();
Bounce2::Button btn_b = Bounce2::Button();
Bounce2::Button end_1 = Bounce2::Button();
Bounce2::Button end_2 = Bounce2::Button();
Bounce2::Button end_3 = Bounce2::Button();

// Potentiometers
averageFilter poti_a_filtered, poti_b_filtered;
int32_t poti_a, poti_a_old;
int32_t poti_b, poti_b_old;

// Timing
unsigned long current_millis;
unsigned long previous_millis;

unsigned long current_millis_serial_received;
unsigned long previous_millis_serial_received;

float fps = 60;

// Target Positions for Steppers and Servos
long targetPositions[14];

Logger Log;
SDCardHelpers SDCard;
Dynamixel Dyna;
NeoPixels NeoPix;
#endif