#ifndef STEPPERWRAPPER_H
#define STEPPERWRAPPER_H

#include <Arduino.h>
#include <AccelStepper.h>
#include <Bounce2.h>
#include "../averageFilter.h"

// Stepper Motor Pins:
#define STEPPER_3_DIR 0
#define STEPPER_3_PULSE 1
#define STEPPER_2_DIR 2
#define STEPPER_2_PULSE 3
#define STEPPER_1_DIR 4
#define STEPPER_1_PULSE 5
#define STEPPER_ENABLE 9
#define STEPPER_3_END 6
#define STEPPER_2_END 7
#define STEPPER_1_END 8

class StepperWrapper
{
private:
    AccelStepper stepper_1;
    AccelStepper stepper_2;
    AccelStepper stepper_3;
    // AccelStepper stepper_1(AccelStepper::DRIVER, STEPPER_1_PULSE, STEPPER_1_DIR);
    // AccelStepper stepper_2(AccelStepper::DRIVER, STEPPER_2_PULSE, STEPPER_2_DIR);
    // AccelStepper stepper_3(AccelStepper::DRIVER, STEPPER_3_PULSE, STEPPER_3_DIR);

    Bounce2::Button end_1;
    Bounce2::Button end_2;
    Bounce2::Button end_3;
    // Bounce2::Button end_1 = Bounce2::Button();
    // Bounce2::Button end_2 = Bounce2::Button();
    // Bounce2::Button end_3 = Bounce2::Button();

    averageFilter stepper_1_speed_filtered;
    averageFilter stepper_2_speed_filtered;
    averageFilter stepper_3_speed_filtered;

    const float maxSpeed_Offset = 10000; // turn all a bit slower...
    // we figured out that the values below do work with our robot,
    // but it's super fast and stresses the mechanics quite a lot
    const float stepper_1_maxSpeed = 30000 - maxSpeed_Offset;
    const float stepper_2_maxSpeed = 20000 - maxSpeed_Offset;
    const float stepper_3_maxSpeed = 20000 - maxSpeed_Offset;

    unsigned int minPulseWidth = 3;

    long targetPositions[3];

public:
    StepperWrapper();
    void zeroPositions();
    void setNewStepperPositions();
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