#ifndef PINS_H
#define PINS_H

#include <Arduino.h>
#include "AccelStepper/AccelStepper.h"
#include "Dynamixel2Arduino/Dynamixel2Arduino.h"
// #include "LiquidCrystal_I2C/LiquidCrystal_I2C.h"
#include "Bounce2/Bounce2.h"

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

// Pins for Board Version 2.0
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

// Dynamixel Servos:
#define DXL_SERIAL Serial5
#define DXL_DIR_PIN 22

// Buttons + Potis:
#define BTN_A 14
#define POTI_A 15
#define POTI_B 16
#define BTN_B 17

// Display
// LiquidCrystal_I2C lcd(0x27, 20, 4); // set the LCD address to 0x27 for a 16 chars and 2 line display

// SD-Card
const int chipSelect = BUILTIN_SDCARD;
File root;

// NeoPixel LEDs
const int numled = 16;
const int pin = 24;
byte drawingMemory[numled * 4];         //  4 bytes per LED for RGBW
DMAMEM byte displayMemory[numled * 16]; // 16 bytes per LED for RGBW
WS2812Serial leds(numled, displayMemory, drawingMemory, pin, WS2812_GRBW);
#define RED 0x00FF0000
#define GREEN 0x0000FF00
#define BLUE 0x000000FF
#define YELLOW 0x00FFD000
#define PINK 0x44F00080
#define ORANGE 0x00FF4200
#define WHITE 0xAA000000


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

// Dynamixel Servos
uint8_t DXL_ID = 0;
const float DXL_PROTOCOL_VERSION = 2.0;
const uint8_t BROADCAST_ID = 254;
const uint8_t DXL_ID_CNT = 11;
const uint8_t DXL_ID_LIST[DXL_ID_CNT] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
const uint16_t user_pkt_buf_cap = 128;
uint8_t user_pkt_buf[user_pkt_buf_cap];

const uint16_t PRESENT_POS_ADDR = 132;
const uint16_t PRESENT_POS_LEN = 4;
const uint16_t GOAL_POS_ADDR = 116;
const uint16_t GOAL_POS_LEN = 4;

typedef struct sr_data
{ // Sync Read Data
    int32_t present_position;
} __attribute__((packed)) sr_data_t;

typedef struct sw_data
{ // Sync Write Data
    int32_t goal_position;
} __attribute__((packed)) sw_data_t;

sr_data_t sr_data[DXL_ID_CNT]; // Sync Read Data
DYNAMIXEL::InfoSyncReadInst_t sr_infos;
DYNAMIXEL::XELInfoSyncRead_t info_xels_sr[DXL_ID_CNT];

sw_data_t sw_data[DXL_ID_CNT]; // Sync Write Data
DYNAMIXEL::InfoSyncWriteInst_t sw_infos;
DYNAMIXEL::XELInfoSyncWrite_t info_xels_sw[DXL_ID_CNT];

Dynamixel2Arduino dxl(DXL_SERIAL, DXL_DIR_PIN);

const uint8_t goal_position_count = 4;                                                   // temporary for test
const int32_t goal_position[goal_position_count] = {2048, 2048 - 256, 2048, 2048 + 256}; // temporary for test
uint8_t goal_position_index = 0;                                                         // temporary for test

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

#endif