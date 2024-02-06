#ifndef DYNAMIXEL_H
#define DYNAMIXEL_H

#include <Arduino.h>
// #include "../Dynamixel2Arduino/Dynamixel2Arduino.h"
#include <Dynamixel2Arduino.h>
// #include "pins.h"

// Dynamixel Servos
// #define DXL_DIR_PIN 22
// #define DXL_SERIAL Serial5

class Dynamixel
{
private:
    // static const uint8_t DXL_DIR_PIN = 22;
    // static String DXL_SERIAL = "Serial5";
    // const Dynamixel2Arduino dxl(Serial5, 22);

    static Dynamixel2Arduino *dxl;

    static constexpr float DXL_PROTOCOL_VERSION = 2.0;
    static const uint8_t BROADCAST_ID = 254;
    static const uint8_t DXL_ID_CNT = 11;
    static const uint8_t DXL_ID_LIST[DXL_ID_CNT];

    static const uint16_t user_pkt_buf_cap = 128;
    static uint8_t user_pkt_buf[user_pkt_buf_cap];

    // Byte-Addresses for Dynamixel Commands:
    static const uint16_t PRESENT_POS_ADDR = 132;
    static const uint16_t PRESENT_POS_LEN = 4;
    static const uint16_t GOAL_POS_ADDR = 116;
    static const uint16_t GOAL_POS_LEN = 4;

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

public:
    // Dynamixel(HardwareSerial &port, int dir_pin);
    Dynamixel(Dynamixel2Arduino &dxl);
    void setNewDynamixelPositions(long *targetPositions);
    void DynamixelTest();
    void enableTorque();
    void disableTorque();
    void enableLEDs();
    void disableLEDs();
    void rebootDynamixels();
    void prepareSyncRead();
    void prepareSyncWrite();
    void init_dxl();
};

#endif