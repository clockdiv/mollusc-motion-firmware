#ifndef STEPPERWRAPPER_H
#define STEPPERWRAPPER_H

#include <Arduino.h>
#include <AccelStepper.h>
#include <Bounce2.h>
#include "../averageFilter.h"

#define STEPPER_COUNT 4

#define STEPPER_0_DIR 4
#define STEPPER_0_PULSE 5
#define STEPPER_0_END 6

#define STEPPER_1_DIR 2
#define STEPPER_1_PULSE 3
#define STEPPER_1_END 7

#define STEPPER_2_DIR 0
#define STEPPER_2_PULSE 1
#define STEPPER_2_END 8

#define STEPPER_3_DIR 10
#define STEPPER_3_PULSE 11
#define STEPPER_3_END 12

#define STEPPER_ENABLE 9

#define HOMING_TIMEOUT true

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

    const uint8_t pulsePins[STEPPER_COUNT] = {STEPPER_0_PULSE, STEPPER_1_PULSE, STEPPER_2_PULSE, STEPPER_3_PULSE};
    const uint8_t directionPins[STEPPER_COUNT] = {STEPPER_0_DIR, STEPPER_1_DIR, STEPPER_2_DIR, STEPPER_3_DIR};
    const uint8_t limitswitchPins[STEPPER_COUNT] = {STEPPER_0_END, STEPPER_1_END, STEPPER_2_END, STEPPER_3_END};

    float stepperMaxSpeed[STEPPER_COUNT];
    float stepperAcceleration[STEPPER_COUNT];
    const long fps = 60; // redundant
    const unsigned int minPulseWidth = 3;

    elapsedMillis homingTimeoutTimer;
    const unsigned long homingTimeout = 8000;

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