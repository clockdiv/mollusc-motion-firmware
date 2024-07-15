#include "StateManager.h"

const String StateManager::states[] = {"STARTUP",
                                       "IDLE",
                                       "PLAYING",
                                       "RUNNING",
                                       "MANUAL",
                                       "HOMING_A",
                                       "HOMING_B"};

States StateManager::current_state = STARTUP;
States StateManager::state_old = STARTUP;

StepperWrapper *StateManager::stepperWrapper;
Dynamixel *StateManager::dynamixel;

void StateManager::setStepperWrapper(StepperWrapper *extStepperWrapper)
{
    stepperWrapper = extStepperWrapper;
}

void StateManager::setDynamixel(Dynamixel *extDynamixel)
{
    dynamixel = extDynamixel;
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
        case States::STARTUP:
            break;
        case States::IDLE:
            stepperWrapper->initIdle();
            break;
        case States::PLAYING:
            stepperWrapper->initPlay();
            break;
        case States::RUNNING:
            stepperWrapper->initRunning();
            break;
        case States::MANUAL:
            stepperWrapper->initManual();
            break;
        case States::HOMING_A:
            stepperWrapper->initHoming_A();
            dynamixel->rebootDynamixels();
            dynamixel->disableTorque();
            dynamixel->enableLEDs();
            break;
        case States::HOMING_B:
            stepperWrapper->zeroPositions();
            stepperWrapper->initHoming_B();

            dynamixel->disableLEDs();
            dynamixel->enableTorque();

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

void StateManager::setStateFromString(String new_state)
{
    if (new_state == "RUNNING")
    {
        current_state = States::RUNNING;
    }
    else if (new_state == "MANUAL")
    {
        current_state = States::MANUAL;
    }
    else if (new_state == "IDLE")
    {
        current_state = States::IDLE;
    }
    else if (new_state == "HOMING_A")
    {
        current_state = States::HOMING_A;
    }
    else
    {
        current_state = States::IDLE;
    }
    handleStateChange();
}
