#ifndef STEPPERWRAPPER_H
#define STEPPERWRAPPER_H

#include <Arduino.h>
#include <AccelStepper.h>
#include <Bounce2.h>
#include "../averageFilter.h"

#define STEPPER_COUNT 4

class StepperWrapper
{
private:
    // AccelStepper stepper_0;
    // AccelStepper stepper_1;
    // AccelStepper stepper_2;


    // averageFilter stepper_0_speed_filtered;
    // averageFilter stepper_1_speed_filtered;
    // averageFilter stepper_2_speed_filtered;

    averageFilter stepperSpeedFiltered[STEPPER_COUNT];

    float stepperMaxSpeed[STEPPER_COUNT];
    float stepperAcceleration[STEPPER_COUNT];
    const long fps = 60; // redundant
    const unsigned int minPulseWidth = 3;

public:
    // Bounce2::Button limit_switch_0;
    // Bounce2::Button limit_switch_1;
    // Bounce2::Button limit_switch_2;

    const uint8_t stepperCount = STEPPER_COUNT;
    AccelStepper stepper[STEPPER_COUNT];
    Bounce2::Button limitSwitch[STEPPER_COUNT];

    StepperWrapper();
    void setDirPins(bool dir_step_0, bool dir_step_1, bool dir_step_2, bool dir_step_3);
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