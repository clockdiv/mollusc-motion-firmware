#include "StateManager.h"

const String StateManager::states[] = {"STARTUP",
                                       "IDLE",
                                       "RUNNING",
                                       "MANUAL",
                                       "HOMING_A",
                                       "HOMING_B"};

StateManager::StateManager()
{
    current_state = States::STARTUP;
    state_old = States::STARTUP;
}

String StateManager::getStateAsString()
{
    return states[current_state];
}

States StateManager::getState()
{
    return current_state;
}

void StateManager::handleStateChange()
{
    // Print, if we entered a new state
    if (current_state != state_old)
    {
        // log_sd("changed state to: " + getStateAsString());
        switch (current_state)
        {
        case States::IDLE:
            // initIdle(); // TODO enable again!
            break;
        case States::RUNNING:
            // initRunning();// TODO enable again!
            break;
        case States::MANUAL:
            // initManual();// TODO enable again!
            break;
        case States::HOMING_A:
            // initHoming_A();// TODO enable again!
            break;
        case States::HOMING_B:
            // initHoming_B();// TODO enable again!
            break;
        default:
            break;
        }
        state_old = current_state;
    }
}

void StateManager::setState(States state)
{
    // log_sd("set_state()");
    // log_sd("current state: " + getStateAsString());
    current_state = state;
    // log_sd("new state: " + getStateAsString());
    handleStateChange();
}

// void StateManager::handle_current_state()
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
