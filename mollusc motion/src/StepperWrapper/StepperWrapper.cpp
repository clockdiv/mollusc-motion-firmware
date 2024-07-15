#include "StepperWrapper.h"

#define IGNORE_LIMIT_SWITCHES true

StepperWrapper::StepperWrapper()
{
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
    pinMode(STEPPER_ENABLE, OUTPUT);
    digitalWriteFast(STEPPER_ENABLE, LOW); // enable all steppers
}

void StepperWrapper::setDirPins(bool dir_step_0, bool dir_step_1, bool dir_step_2, bool dir_step_3)
{
    stepper[0].setPinsInverted(dir_step_0, false, false); // direction, step, enabled
    stepper[1].setPinsInverted(dir_step_1, false, false); // direction, step, enabled
    stepper[2].setPinsInverted(dir_step_2, false, false); // direction, step, enabled
    stepper[3].setPinsInverted(dir_step_3, false, false); // direction, step, enabled
}

void StepperWrapper::zeroPositions()
{
    for (uint8_t i = 0; i < STEPPER_COUNT; i++)
    {
        stepper[i].setCurrentPosition(0);
    }
}

void StepperWrapper::setNewStepperPositions(long *targetPositions, float lerpValue)
{
    for (uint8_t i = 0; i < STEPPER_COUNT; i++)
    {
        uint16_t lerpedTarget = lerp(stepper[i].currentPosition(), targetPositions[i], lerpValue);
        stepper[i].moveTo(lerpedTarget);
        // stepper[i].moveTo(targetPositions[i]); // direct target, not lerped
        float speedSteeper = stepperSpeedFiltered[i].filter(fps * float(stepper[i].distanceToGo()) * 0.1f);
        stepper[i].setSpeed(speedSteeper);
        // Serial.print("\tID: ");
        // Serial.print(i);
        // Serial.print("\tcurrent/lerped/target: ");
        // Serial.print(stepper[i].currentPosition());
        // Serial.print("/");
        // Serial.print(lerpedTarget);
        // Serial.print("/");
        // Serial.print(targetPositions[i]);
        // Serial.print("\t");
        // Serial.println(speedSteeper);
    }
    // Serial.println();
}

void StepperWrapper::initIdle()
{
    // Serial.println("initIdle()");
    for (uint8_t i = 0; i < STEPPER_COUNT; i++)
    {
        stepper[i].setSpeed(0);
    }
}

void StepperWrapper::initRunning()
{
    // Serial.println("initRunning()");

    for (uint8_t i = 0; i < STEPPER_COUNT; i++)
    {
        stepper[i].setMaxSpeed(stepperMaxSpeed[i]);
    }
}

void StepperWrapper::initPlay()
{
    // Serial.println("initPlay()");

    for (uint8_t i = 0; i < STEPPER_COUNT; i++)
    {
        stepper[i].setMaxSpeed(stepperMaxSpeed[i]);
    }
}

void StepperWrapper::initManual()
{
    // this is called when the user 'jumps' within the
    // timeline in Blender. The motors are moved with
    // acceleration to their target positions.
    // Serial.println("initManual()");

    // all values for MaxSpeed and Acceleration tested
    // Stepper 1 runs with 800 MicroSteps
    // Stepper 2 runs with 1600 MicroSteps
    // Stepper 3 runs with 1600 MicroSteps
    for (uint8_t i = 0; i < STEPPER_COUNT; i++)
    {
        stepper[i].setMaxSpeed(stepperMaxSpeed[i] / 16);
        stepper[i].setAcceleration(stepperAcceleration[i] / 16);
    }
}

void StepperWrapper::initHoming_A()
{
    // Serial.println("initHoming_A()");
    for (uint8_t i = 0; i < STEPPER_COUNT; i++)
    {
        stepper[i].setAcceleration(200);
        stepper[i].setMaxSpeed(500);
        stepper[i].setSpeed(-500);
    }

    homingTimeoutTimer = 0;
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
}

void StepperWrapper::initHoming_B()
{
    // Serial.println("initHoming_B()");
    for (uint8_t i = 0; i < STEPPER_COUNT; i++)
    {
        stepper[i].setMaxSpeed(1600);
        stepper[i].setAcceleration(1000);
        stepper[i].move(800);
    }

    stepper[0].move(2000); // Body
    stepper[1].move(3500); // Neck
    stepper[2].move(1500); // Head Left
    stepper[3].move(1500); // Head Right
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

    return done;
}

void StepperWrapper::updateEndSwitches()
{
    for (uint8_t i = 0; i < STEPPER_COUNT; i++)
    {
        limitSwitch[i].update();
    }
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
    }
    // Serial.println("Run all speed to positions");
    for (uint8_t i = 0; i < STEPPER_COUNT; i++)
    {
        stepper[i].runSpeedToPosition();
    }
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
    }
    // Serial.println("Run all");
    for (uint8_t i = 0; i < STEPPER_COUNT; i++)
    {
        stepper[i].run();
    }
}

float StepperWrapper::lerp(float a, float b, float f)
{
    return a * (1.0 - f) + (b * f);
}