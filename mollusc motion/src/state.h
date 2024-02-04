#ifndef STATE_H
#define STATE_H

#include "display.h"
#include "steppers.h"

enum State
{
    STARTUP,
    IDLE,
    RUNNING,
    MANUAL,
    HOMING_A, // drive to the end-switches
    HOMING_B  // moves away from the end-switches n steps and sets zero-position
};
State state = STARTUP, state_old = STARTUP;

void print_state()
{
    Serial.print("NEW STATE: ");
    switch (state)
    {
    case STARTUP:
        Serial.println("STARTUP");
        break;
    case IDLE:
        Serial.println("IDLE");
        break;
    case RUNNING:
        Serial.println("RUNNING");
        break;
    case MANUAL:
        Serial.println("MANUAL");
        break;
    case HOMING_A:
        Serial.println("HOMING_A (Drive to end-switches).");
        break;
    case HOMING_B:
        Serial.println("HOMING_B (Drive forward and set zero-position).");
        break;
    default:
        break;
    }
}

void print_state_lcd()
{
    // lcd.clear();
    // lcd.setCursor(0, 0);

    // switch (state)
    // {
    // case STARTUP:
    //     lcd.print("STARTUP");
    //     break;
    // case IDLE:
    //     lcd.print("IDLE");
    //     break;
    // case RUNNING:
    //     lcd.print("RUNNING");
    //     break;
    // case MANUAL:
    //     lcd.print("MANUAL");
    //     break;
    // case HOMING_A:
    //     lcd.print("HOMING_A");
    //     break;
    // case HOMING_B:
    //     lcd.print("HOMING_B");
    //     break;
    // default:
    //     break;
    // }
}

void handle_state_change()
{
    // Print, if we entered a new state
    if (state != state_old)
    {
        print_state();
        print_state_lcd();
        switch (state)
        {
        case IDLE:
            initIdle();
            break;
        case RUNNING:
            initRunning();
            break;
        case MANUAL:
            initManual();
            break;
        case HOMING_A:
            initHoming_A();
            break;
        case HOMING_B:
            initHoming_B();
            break;
        default:
            break;
        }
        state_old = state;
    }
}
void set_state(State new_state)
{
    state = new_state;
    handle_state_change();
}

// void handle_current_state()
// {
//     // Switch to IDLE if all tasks are done
//     if (stepper_1.distanceToGo() == 0 &&
//         stepper_2.distanceToGo() == 0 &&
//         stepper_3.distanceToGo() == 0 &&
//         state != IDLE &&
//         state != HOMING_A &&
//         state != HOMING_B)
//     {
//         stepper_1.setSpeed(0.0f);
//         stepper_2.setSpeed(0.0f);
//         stepper_3.setSpeed(0.0f);
//         set_state(IDLE);
//     }
// }
#endif