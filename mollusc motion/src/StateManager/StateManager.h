#ifndef STATEMANAGER_H
#define STATEMANAGER_H

#include <Arduino.h>
#include "../StepperWrapper/StepperWrapper.h"

enum States
{
    STARTUP,
    IDLE,
    PLAYING,
    RUNNING,
    MANUAL,
    HOMING_A, // drive to the end-switches
    HOMING_B  // moves away from the end-switches n steps and sets zero-position
};

class StateManager
{
private:
    static const String states[];
    static States current_state;
    static States state_old;

    static StepperWrapper *stepperWrapper;

public:
    static void setStepperWrapper(StepperWrapper *extStepperWrapper);

    static String getStateAsString();
    static States getState();
    static void handleStateChange();
    static void setState(States new_state);
    static void setStateFromString(String new_state);
    // void handle_current_state();
};

#endif