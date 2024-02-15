#ifndef STEPPERWRAPPER_H
#define STEPPERWRAPPER_H

#include <Arduino.h>
#include <AccelStepper.h>
#include <Bounce2.h>
#include "../averageFilter.h"

// Stepper Motor Pins:
#define STEPPER_0_DIR 4
#define STEPPER_1_DIR 2
#define STEPPER_2_DIR 0

#define STEPPER_0_PULSE 5
#define STEPPER_1_PULSE 3
#define STEPPER_2_PULSE 1

#define STEPPER_0_END 6
#define STEPPER_1_END 7
#define STEPPER_2_END 8

#define STEPPER_ENABLE 9

class StepperWrapper
{
private:
    AccelStepper stepper_0;
    AccelStepper stepper_1;
    AccelStepper stepper_2;

    averageFilter stepper_0_speed_filtered;
    averageFilter stepper_1_speed_filtered;
    averageFilter stepper_2_speed_filtered;

    const float maxSpeed_Offset = 10000; // turn all a bit slower...
    // we figured out that the values below do work with our robot,
    // but it's super fast and stresses the mechanics quite a lot
    const float stepper_0_maxSpeed = 30000 - maxSpeed_Offset;
    const float stepper_1_maxSpeed = 20000 - maxSpeed_Offset;
    const float stepper_2_maxSpeed = 20000 - maxSpeed_Offset;

    unsigned int minPulseWidth = 3;

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
    void initManual();
    void initHoming_A();
    bool driveHoming_A();
    void initHoming_B();
    bool driveHoming_B();
    void updateEndSwitches();
    void runAllSpeedToPositions();
    void runAll();
};

#endif