#ifndef STEPPERWRAPPER_H
#define STEPPERWRAPPER_H

#include <Arduino.h>
#include <AccelStepper.h>
#include <Bounce2.h>
#include "../averageFilter.h"

class StepperWrapper
{
private:
    AccelStepper stepper_0;
    AccelStepper stepper_1;
    AccelStepper stepper_2;

    averageFilter stepper_0_speed_filtered;
    averageFilter stepper_1_speed_filtered;
    averageFilter stepper_2_speed_filtered;

    const long fps = 60; // redundant

public:
    Bounce2::Button limit_switch_0;
    Bounce2::Button limit_switch_1;
    Bounce2::Button limit_switch_2;

    StepperWrapper();
    void setDirPins(bool dir_step_0, bool dir_step_1, bool dir_step_2);
    void zeroPositions();
    void setNewStepperPositions(long *targetPositions);
    void initIdle();
    void initRunning();
    void initPlay();
    void initManual();
    void initHoming_A();
    bool driveHoming_A();
    void initHoming_B();
    bool driveHoming_B();
    void updateEndSwitches();
    void printEndSwitches();
    void runAllSpeedToPositions();
    void runAll();
    void calcSpeed();
};

#endif