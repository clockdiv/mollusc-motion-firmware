#include "Dynamixel.h"

constexpr uint8_t Dynamixel::DXL_ID_LIST[DXL_ID_CNT] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
uint8_t Dynamixel::user_pkt_buf[Dynamixel::user_pkt_buf_cap];
Dynamixel2Arduino Dynamixel::dxl;

Dynamixel::Dynamixel()
{
    dxl = Dynamixel2Arduino(Serial5, 22);
}

void Dynamixel::setNewDynamixelPositions()
{
    // unsigned long dynamixelWriteDuration = micros();
    for (uint8_t i = 0; i < DXL_ID_CNT; i++)
    {
        sw_data[i].goal_position = targetPositions[i];
    }
    // Update the SyncWrite packet status
    sw_infos.is_info_changed = true;

    // Build a SyncWrite Packet and transmit to DYNAMIXEL
    if (!dxl.syncWrite(&sw_infos))
    {
        Serial.print("[SyncWrite] Fail, Lib error code: ");
        Serial.println(dxl.getLastLibErrCode());
    }
    // dynamixelWriteDuration = micros() - dynamixelWriteDuration;
    // Serial.println(dynamixelWriteDuration);
}

void Dynamixel::DynamixelTest()
{
    if (current_millis - previous_millis > 2000)
    {
        // Insert a new Goal Position to the SyncWrite Packet
        for (uint8_t i = 0; i < DXL_ID_CNT; i++)
        {
            sw_data[i].goal_position = goal_position[goal_position_index];
        }

        // Update the SyncWrite packet status
        sw_infos.is_info_changed = true;

        // Build a SyncWrite Packet and transmit to DYNAMIXEL
        if (dxl.syncWrite(&sw_infos) == true)
        {
            Serial.println("[SyncWrite] Success");
            for (uint8_t i = 0; i < sw_infos.xel_count; i++)
            {
                Serial.print("  ID: ");
                Serial.print(sw_infos.p_xels[i].id);
                Serial.print(",\tGoal Position: ");
                Serial.println(sw_data[i].goal_position);
            }
            goal_position_index++;
            if (goal_position_index == 4)
                goal_position_index = 0;
        }
        else
        {
            Serial.print("[SyncWrite] Fail, Lib error code: ");
            Serial.print(dxl.getLastLibErrCode());
        }
        Serial.println();

        // Transmit predefined SyncRead instruction packet
        // and receive a status packet from each DYNAMIXEL
        uint8_t recv_cnt = dxl.syncRead(&sr_infos);
        if (recv_cnt > 0)
        {
            Serial.print("[SyncRead] Success, Received ID Count: ");
            Serial.println(recv_cnt);
            for (uint8_t i = 0; i < recv_cnt; i++)
            {
                Serial.print("  ID: ");
                Serial.print(sr_infos.p_xels[i].id);
                Serial.print(",\tError: ");
                Serial.print(sr_infos.p_xels[i].error);
                Serial.print(",\tPresent Position: ");
                Serial.println(sr_data[i].present_position);
            }
        }
        else
        {
            Serial.print("[SyncRead] Fail, Lib error code: ");
            Serial.println(dxl.getLastLibErrCode());
        }
        Serial.println("=======================================================");
        previous_millis = current_millis;
    }
}

void Dynamixel::enableTorque()
{
    // Enable Torque on all Servos
    for (uint8_t i = 0; i < DXL_ID_CNT; i++)
    {
        dxl.torqueOff(DXL_ID_LIST[i]);
        dxl.setOperatingMode(DXL_ID_LIST[i], OP_POSITION);
    }
    dxl.torqueOn(BROADCAST_ID);
    dxl.writeControlTableItem(ControlTableItem::PROFILE_VELOCITY, BROADCAST_ID, 0);
}

void Dynamixel::disableTorque()
{
    // Disable Torque on all Servos
    for (uint8_t i = 0; i < DXL_ID_CNT; i++)
    {
        dxl.torqueOff(DXL_ID_LIST[i]);
        // dxl.setOperatingMode(DXL_ID_LIST[i], OP_POSITION);
    }

    dxl.torqueOff(BROADCAST_ID);
    dxl.writeControlTableItem(ControlTableItem::PROFILE_VELOCITY, BROADCAST_ID, 0);
}

void Dynamixel::enableLEDs()
{
    dxl.ledOn(BROADCAST_ID);
    // dxl.writeControlTableItem(ControlTableItem::LED, BROADCAST_ID, 1);
}

void Dynamixel::disableLEDs()
{
    dxl.ledOff(BROADCAST_ID);
    // dxl.writeControlTableItem(ControlTableItem::LED, BROADCAST_ID, 0);
}

void Dynamixel::rebootDynamixels()
{
    dxl.reboot(BROADCAST_ID);
}

void Dynamixel::prepareSyncRead()
{
    // Fill the members of structure to syncRead using external user packet buffer
    sr_infos.packet.p_buf = user_pkt_buf;
    sr_infos.packet.buf_capacity = user_pkt_buf_cap;
    sr_infos.packet.is_completed = false;
    sr_infos.addr = PRESENT_POS_ADDR;
    sr_infos.addr_length = PRESENT_POS_LEN;
    sr_infos.p_xels = info_xels_sr;
    sr_infos.xel_count = 0;

    for (uint8_t i = 0; i < DXL_ID_CNT; i++)
    {
        info_xels_sr[i].id = DXL_ID_LIST[i];
        info_xels_sr[i].p_recv_buf = (uint8_t *)&sr_data[i];
        sr_infos.xel_count++;
    }
    sr_infos.is_info_changed = true;
}

void Dynamixel::prepareSyncWrite()
{
    // Fill the members of structure to syncWrite using internal packet buffer
    sw_infos.packet.p_buf = nullptr;
    sw_infos.packet.is_completed = false;
    sw_infos.addr = GOAL_POS_ADDR;
    sw_infos.addr_length = GOAL_POS_LEN;
    sw_infos.p_xels = info_xels_sw;
    sw_infos.xel_count = 0;

    for (uint8_t i = 0; i < DXL_ID_CNT; i++)
    {
        info_xels_sw[i].id = DXL_ID_LIST[i];
        info_xels_sw[i].p_data = (uint8_t *)&sw_data[i].goal_position;
        sw_infos.xel_count++;
    }
    sw_infos.is_info_changed = true;
}

void Dynamixel::init_dxl()
{
    // dxl.begin(57600);
    dxl.begin(2000000);
    dxl.setPortProtocolVersion(DXL_PROTOCOL_VERSION);

    enableTorque();
    prepareSyncRead();
    prepareSyncWrite();

    // dxl.ping(ID);
    // // Turn off torque when configuring items in EEPROM area
    // dxl.torqueOff(ID);
    // dxl.setOperatingMode(ID, OP_POSITION);

    // dxl.torqueOn(ID);
    // // Limit the maximum velocity in Position Control Mode. Use 0 for Max speed
    // dxl.writeControlTableItem(ControlTableItem::PROFILE_VELOCITY, ID, 30);
}
