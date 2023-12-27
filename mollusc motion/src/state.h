#ifndef STATE_H
#define STATE_H

#include "mm_sdcard.h"
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

String states[] = {"STARTUP",
                   "IDLE",
                   "RUNNING",
                   "MANUAL",
                   "HOMING_A",
                   "HOMING_B"};

String get_current_state_as_string()
{
    return states[state];
}

void handle_state_change()
{
    // Print, if we entered a new state
    if (state != state_old)
    {
        log_sd("changed state to: " + get_current_state_as_string());
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
    log_sd("set_state()");
    log_sd("current state: " + get_current_state_as_string());
    state = new_state;
    log_sd("new state: " + get_current_state_as_string());
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