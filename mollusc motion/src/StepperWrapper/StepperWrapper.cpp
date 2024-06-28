#include "StepperWrapper.h"

#define IGNORE_LIMIT_SWITCHES true

// Stepper Motor Pins:

// const float maxSpeed_Offset = 10000; // turn all a bit slower...
// // we figured out that the values below do work with our robot,
// // but it's super fast and stresses the mechanics quite a lot
// const float stepper_0_maxSpeed = 30000 - maxSpeed_Offset;
// const float stepper_1_maxSpeed = 20000 - maxSpeed_Offset;
// const float stepper_2_maxSpeed = 20000 - maxSpeed_Offset;

// const unsigned int minPulseWidth = 3;

StepperWrapper::StepperWrapper()
{

    // stepper_0 = AccelStepper(AccelStepper::DRIVER, STEPPER_0_PULSE, STEPPER_0_DIR);
    // stepper_1 = AccelStepper(AccelStepper::DRIVER, STEPPER_1_PULSE, STEPPER_1_DIR);
    // stepper_2 = AccelStepper(AccelStepper::DRIVER, STEPPER_2_PULSE, STEPPER_2_DIR);

    // stepper[0] = AccelStepper(AccelStepper::DRIVER, STEPPER_0_PULSE, STEPPER_0_DIR);
    // stepper[1] = AccelStepper(AccelStepper::DRIVER, STEPPER_1_PULSE, STEPPER_1_DIR);
    // stepper[2] = AccelStepper(AccelStepper::DRIVER, STEPPER_2_PULSE, STEPPER_2_DIR);
    // stepper[3] = AccelStepper(AccelStepper::DRIVER, STEPPER_3_PULSE, STEPPER_3_DIR);

    for (uint8_t i = 0; i < STEPPER_COUNT; i++)
    {
        stepper[i] = AccelStepper(AccelStepper::DRIVER, pulsePins[i], directionPins[i]);
        stepper[i].setMinPulseWidth(minPulseWidth);

        stepperMaxSpeed[i] = 20000;
        stepperAcceleration[i] = 60000;

        limitSwitch[i] = Bounce2::Button();
        limitSwitch[i].interval(25);
        limitSwitch[i].setPressedState(LOW);
        limitSwitch[i].attach(limitswitchPins[i], INPUT_PULLUP);
    }
    // stepper_0.setMinPulseWidth(minPulseWidth);
    // stepper_0.setPinsInverted(false, false, false); // direction, step, enabled

    // stepper_1.setMinPulseWidth(minPulseWidth);
    // stepper_1.setPinsInverted(true, false, false); // direction, step, enabled

    // stepper_2.setMinPulseWidth(minPulseWidth);
    // stepper_2.setPinsInverted(false, false, false); // direction, step, enabled

    // limit_switch_0 = Bounce2::Button();
    // limit_switch_1 = Bounce2::Button();
    // limit_switch_2 = Bounce2::Button();

    // limitSwitch[0].attach(STEPPER_0_END, INPUT_PULLUP);
    // limitSwitch[1].attach(STEPPER_1_END, INPUT_PULLUP);
    // limitSwitch[2].attach(STEPPER_2_END, INPUT_PULLUP);
    // limitSwitch[3].attach(STEPPER_3_END, INPUT_PULLUP);

    // limit_switch_0.attach(STEPPER_0_END, INPUT_PULLUP);
    // limit_switch_0.interval(25);
    // limit_switch_0.setPressedState(LOW);

    // limit_switch_1.attach(STEPPER_1_END, INPUT_PULLUP);
    // limit_switch_1.interval(25);
    // limit_switch_1.setPressedState(LOW);

    // limit_switch_2.attach(STEPPER_2_END, INPUT_PULLUP);
    // limit_switch_2.interval(25);
    // limit_switch_2.setPressedState(LOW);

    pinMode(STEPPER_ENABLE, OUTPUT);
    digitalWriteFast(STEPPER_ENABLE, LOW); // enable all steppers
}

void StepperWrapper::setDirPins(bool dir_step_0, bool dir_step_1, bool dir_step_2, bool dir_step_3)
{
    stepper[0].setPinsInverted(dir_step_0, false, false); // direction, step, enabled
    stepper[1].setPinsInverted(dir_step_1, false, false); // direction, step, enabled
    stepper[2].setPinsInverted(dir_step_2, false, false); // direction, step, enabled
    stepper[3].setPinsInverted(dir_step_3, false, false); // direction, step, enabled

    // stepper_0.setPinsInverted(dir_step_0, false, false); // direction, step, enabled
    // stepper_1.setPinsInverted(dir_step_1, false, false); // direction, step, enabled
    // stepper_2.setPinsInverted(dir_step_2, false, false); // direction, step, enabled
}

void StepperWrapper::zeroPositions()
{
    for (uint8_t i = 0; i < STEPPER_COUNT; i++)
    {
        stepper[i].setCurrentPosition(0);
    }
    // stepper_0.setCurrentPosition(0);
    // stepper_1.setCurrentPosition(0);
    // stepper_2.setCurrentPosition(0);
}

void StepperWrapper::setNewStepperPositions(long *targetPositions)
{
    // Serial.printf("Stepper 1 target pos: %d\n", targetPositions[0]);
    for (uint8_t i = 0; i < STEPPER_COUNT; i++)
    {
        stepper[i].moveTo(targetPositions[i]);
        float speedSteeper = stepperSpeedFiltered[i].filter(fps * float(stepper[i].distanceToGo()) * 0.1f);
        stepper[i].setSpeed(speedSteeper);
        // Serial.print("Stepper ID: ");
        // Serial.print(i);
        // Serial.print("\t");
        // Serial.print(targetPositions[i]);
        // Serial.print("\t");
        // Serial.println(speedSteeper);
        // Serial.println();
    }
    // stepper_0.moveTo(targetPositions[0]);
    // stepper_1.moveTo(targetPositions[1]);
    // stepper_2.moveTo(targetPositions[2]);

    // float speed_stepper_0 = stepper_0_speed_filtered.filter(fps * float(stepper_0.distanceToGo()) * 0.1f);
    // stepper_0.setSpeed(speed_stepper_0);

    // float speed_stepper_1 = stepper_1_speed_filtered.filter(fps * float(stepper_1.distanceToGo()) * 0.1f);
    // stepper_1.setSpeed(speed_stepper_1);

    // float speed_stepper_2 = stepper_2_speed_filtered.filter(fps * float(stepper_2.distanceToGo()) * 0.1f);
    // stepper_2.setSpeed(speed_stepper_2);

    // // float speed_stepper_0 = fps * float(stepper_0.distanceToGo());
    // // stepper_0.setSpeed(speed_stepper_0);

    // // float speed_stepper_1 = fps * float(stepper_1.distanceToGo());
    // // stepper_1.setSpeed(speed_stepper_1);

    // // float speed_stepper_2 = fps * float(stepper_2.distanceToGo());
    // // stepper_2.setSpeed(speed_stepper_2);

    // // Serial.printf("Stepper 1 target pos: %d\n", stepper_0.targetPosition());
}

void StepperWrapper::initIdle()
{
    Serial.println("initIdle()");
    for (uint8_t i = 0; i < STEPPER_COUNT; i++)
    {
        stepper[i].setSpeed(0);
    }
    // stepper_0.setSpeed(0);
    // stepper_1.setSpeed(0);
    // stepper_2.setSpeed(0);

    // Serial.println(stepper_0.distanceToGo());
    // Serial.println(stepper_1.distanceToGo());
    // Serial.println(stepper_2.distanceToGo());
    // Serial.println(stepper_0.isRunning());
    // Serial.println(stepper_1.isRunning());
    // Serial.println(stepper_2.isRunning());
}

void StepperWrapper::initRunning()
{
    Serial.println("initRunning()");

    for (uint8_t i = 0; i < STEPPER_COUNT; i++)
    {
        stepper[i].setMaxSpeed(stepperMaxSpeed[i]);
    }

    // stepper_0.setMaxSpeed(stepper_0_maxSpeed);
    // stepper_1.setMaxSpeed(stepper_1_maxSpeed);
    // stepper_2.setMaxSpeed(stepper_2_maxSpeed);
}

void StepperWrapper::initPlay()
{
    Serial.println("initPlay()");

    for (uint8_t i = 0; i < STEPPER_COUNT; i++)
    {
        stepper[i].setMaxSpeed(stepperMaxSpeed[i]);
    }
    // stepper_0.setMaxSpeed(stepper_0_maxSpeed);
    // stepper_1.setMaxSpeed(stepper_1_maxSpeed);
    // stepper_2.setMaxSpeed(stepper_2_maxSpeed);
}

void StepperWrapper::initManual()
{
    // this is called when the user 'jumps' within the
    // timeline in Blender. The motors are moved with
    // acceleration to their target positions.
    Serial.println("initManual()");

    // all values for MaxSpeed and Acceleration tested
    // Stepper 1 runs with 800 MicroSteps
    // Stepper 2 runs with 1600 MicroSteps
    // Stepper 3 runs with 1600 MicroSteps
    for (uint8_t i = 0; i < STEPPER_COUNT; i++)
    {
        stepper[i].setMaxSpeed(stepperMaxSpeed[i] / 2);
        stepper[i].setAcceleration(stepperAcceleration[i]);
    }

    // stepper_0.setMaxSpeed(stepper_0_maxSpeed / 2); // half maximum speed,
    // stepper_1.setMaxSpeed(stepper_1_maxSpeed / 2); // we don't need to stress
    // stepper_2.setMaxSpeed(stepper_2_maxSpeed / 2); // our mechanics more than needed

    // stepper_0.setAcceleration(60000);
    // stepper_1.setAcceleration(40000);
    // stepper_2.setAcceleration(60000);
}

void StepperWrapper::initHoming_A()
{
    Serial.println("initHoming_A()");
    for (uint8_t i = 0; i < STEPPER_COUNT; i++)
    {
        // stepper[i].setAcceleration(20000);
        // stepper[i].setMaxSpeed(2000);
        // stepper[i].setSpeed(-1600);
        stepper[i].setAcceleration(200);
        stepper[i].setMaxSpeed(500);
        stepper[i].setSpeed(-500);
    }

    homingTimeoutTimer = 0;
    // const float acceleration = 10000;

    // stepper_0.setMaxSpeed(2000);
    // stepper_1.setMaxSpeed(2000);
    // stepper_2.setMaxSpeed(2000);

    // stepper_0.setSpeed(-1600);
    // stepper_1.setSpeed(-1600);
    // stepper_2.setSpeed(-1600);
}

bool StepperWrapper::driveHoming_A()
{
    if (HOMING_TIMEOUT && homingTimeoutTimer > homingTimeout)
    {
        Serial.println("homing ended due to timeout");
        return true;
    }
    for (uint8_t i = 0; i < STEPPER_COUNT; i++)
    {
        stepper[i].runSpeed();
    }
    return false;

    updateEndSwitches();

    bool limitSwitchState[STEPPER_COUNT];
    bool limitSwitchesState = true;

    for (uint8_t i = 0; i < STEPPER_COUNT; i++)
    {
        limitSwitchState[i] = limitSwitch[i].isPressed();
        if (!limitSwitchState[i])
        {
            // stepper[i].setSpeed(-500);
            stepper[i].runSpeed();
        }
        limitSwitchesState = limitSwitchesState && limitSwitchState[i];
    }

    if (!IGNORE_LIMIT_SWITCHES)
    {
        return limitSwitchesState;
    }
    else
    {
        return false;
    }

    // bool e0 = limit_switch_0.isPressed();
    // bool e1 = limit_switch_1.isPressed();
    // bool e2 = limit_switch_2.isPressed();
    // if (!e0)
    // {
    //     stepper_0.setSpeed(-1600);
    //     stepper_0.runSpeed();
    // }
    // if (!e1)
    // {
    //     stepper_1.setSpeed(-1600);
    //     stepper_1.runSpeed();
    // }
    // if (!e2)
    // {
    //     stepper_2.setSpeed(-1600);
    //     stepper_2.runSpeed();
    // }

    // return e0 && e1 && e2;
}

void StepperWrapper::initHoming_B()
{
    Serial.println("initHoming_B()");
    // const float acceleration = 1000;
    for (uint8_t i = 0; i < STEPPER_COUNT; i++)
    {
        // stepper[i].setCurrentPosition(0);
        stepper[i].setMaxSpeed(1600);
        stepper[i].setAcceleration(1000);
        stepper[i].move(800);
    }

    stepper[0].move(2000); // Body
    stepper[1].move(3500); // Neck
    stepper[2].move(1500); // Head Left
    stepper[3].move(1500); // Head Right

    // stepper_0.setMaxSpeed(1600);
    // stepper_1.setMaxSpeed(3200);
    // stepper_2.setMaxSpeed(3200);

    // stepper_0.setAcceleration(acceleration);
    // stepper_1.setAcceleration(acceleration);
    // stepper_2.setAcceleration(acceleration);

    // stepper_0.move(800);  // here: 5mm
    // stepper_1.move(1600); // here: 5mm
    // stepper_2.move(800);  // here: 2mm

    // stepper_0.setSpeed(800);
    // stepper_1.setSpeed(500);
    // stepper_2.setSpeed(500);
}

bool StepperWrapper::driveHoming_B()
{
    // Serial.println("driveHoming_B()");
    for (uint8_t i = 0; i < STEPPER_COUNT; i++)
    {
        if (stepper[i].distanceToGo() != 0)
        {

            stepper[i].run();
        }
    }

    // stepper_0.run();
    // stepper_1.run();
    // stepper_2.run();

    // Serial.print(F("current Positions: "));
    // Serial.print(stepper_0.currentPosition());
    // Serial.print("\t");
    // Serial.print(stepper_1.currentPosition());
    // Serial.print("\t");
    // Serial.print(stepper_2.currentPosition());

    // Serial.println();

    bool done = true;
    for (uint8_t i = 0; i < STEPPER_COUNT; i++)
    {
        done = done && stepper[i].distanceToGo() == 0;
    }

    // bool done = stepper_0.distanceToGo() == 0 &&
    //             stepper_1.distanceToGo() == 0 &&
    //             stepper_2.distanceToGo() == 0;

    return done;
}

void StepperWrapper::updateEndSwitches()
{
    for (uint8_t i = 0; i < STEPPER_COUNT; i++)
    {
        limitSwitch[i].update();
    }

    // limit_switch_0.update();
    // limit_switch_1.update();
    // limit_switch_2.update();
}

void StepperWrapper::printEndSwitches()
{
    Serial.print(F("limit switches: "));
    for (uint8_t i = 0; i < STEPPER_COUNT; i++)
    {
        Serial.print(limitSwitch[i].isPressed());
        Serial.print("\t");
    }

    // Serial.print(limit_switch_0.isPressed());
    // Serial.print("\t");
    // Serial.print(limit_switch_1.isPressed());
    // Serial.print("\t");
    // Serial.print(limit_switch_2.isPressed());
    Serial.println();
}

void StepperWrapper::runAllSpeedToPositions()
{
    if (!IGNORE_LIMIT_SWITCHES)
    {
        for (uint8_t i = 0; i < STEPPER_COUNT; i++)
        {
            if (limitSwitch[i].isPressed())
            {
                // Serial.println("Limit switch pressed");
                return;
            }
        }
        // if (limit_switch_0.isPressed() || limit_switch_1.isPressed() || limit_switch_2.isPressed())
        //     return;
    }
    // Serial.println("Run all speed to positions");
    for (uint8_t i = 0; i < STEPPER_COUNT; i++)
    {
        stepper[i].runSpeedToPosition();
    }

    // stepper_0.runSpeedToPosition();
    // stepper_1.runSpeedToPosition();
    // stepper_2.runSpeedToPosition();
}

void StepperWrapper::runAll()
{
    if (!IGNORE_LIMIT_SWITCHES)
    {
        for (uint8_t i = 0; i < STEPPER_COUNT; i++)
        {
            if (limitSwitch[i].isPressed())
            {
                // Serial.println("Limit switch pressed");
                return;
            }
        }
        // if (limit_switch_0.isPressed() || limit_switch_1.isPressed() || limit_switch_2.isPressed())
        //     return;
    }
    // Serial.println("Run all");
    for (uint8_t i = 0; i < STEPPER_COUNT; i++)
    {
        stepper[i].run();
    }
    // stepper_0.run();
    // stepper_1.run();
    // stepper_2.run();
}

// bool StepperWrapper::getEndSwitch_0()
// {
//     return limit_switch_0.fell();
// }
// bool StepperWrapper::getEndSwitch_1() {}
// bool StepperWrapper::getEndSwitch_2() {}
