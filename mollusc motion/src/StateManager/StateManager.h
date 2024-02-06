#ifndef STATEMANAGER_H
#define STATEMANAGER_H

#include <Arduino.h>

enum States
{
    STARTUP,
    IDLE,
    RUNNING,
    MANUAL,
    HOMING_A, // drive to the end-switches
    HOMING_B  // moves away from the end-switches n steps and sets zero-position
};

class StateManager
{
private:
    static const String states[];

    States current_state;
    States state_old;

public:
    StateManager();
    String getStateAsString();
    States getState();
    void handleStateChange();
    void setState(States new_state);
    // void handle_current_state();
};

#endif