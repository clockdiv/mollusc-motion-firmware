#include "StepperWrapper.h"

StepperWrapper::StepperWrapper()
{

    stepper_0 = AccelStepper(AccelStepper::DRIVER, STEPPER_0_PULSE, STEPPER_0_DIR);
    stepper_1 = AccelStepper(AccelStepper::DRIVER, STEPPER_1_PULSE, STEPPER_1_DIR);
    stepper_2 = AccelStepper(AccelStepper::DRIVER, STEPPER_2_PULSE, STEPPER_2_DIR);

    stepper_0.setMinPulseWidth(minPulseWidth);
    // stepper_0.setPinsInverted(false, false, false); // direction, step, enabled

    stepper_1.setMinPulseWidth(minPulseWidth);
    // stepper_1.setPinsInverted(true, false, false); // direction, step, enabled

    stepper_2.setMinPulseWidth(minPulseWidth);
    // stepper_2.setPinsInverted(false, false, false); // direction, step, enabled

    limit_switch_0 = Bounce2::Button();
    limit_switch_1 = Bounce2::Button();
    limit_switch_2 = Bounce2::Button();

    limit_switch_0.attach(STEPPER_0_END, INPUT_PULLUP);
    limit_switch_0.interval(25);
    limit_switch_0.setPressedState(LOW);

    limit_switch_1.attach(STEPPER_1_END, INPUT_PULLUP);
    limit_switch_1.interval(25);
    limit_switch_1.setPressedState(LOW);

    limit_switch_2.attach(STEPPER_2_END, INPUT_PULLUP);
    limit_switch_2.interval(25);
    limit_switch_2.setPressedState(LOW);

    pinMode(STEPPER_ENABLE, OUTPUT);
    digitalWriteFast(STEPPER_ENABLE, LOW); // enable all steppers
}

void StepperWrapper::setDirPins(bool dir_step_0, bool dir_step_1, bool dir_step_2)
{
    stepper_0.setPinsInverted(dir_step_0, false, false); // direction, step, enabled
    stepper_1.setPinsInverted(dir_step_1, false, false); // direction, step, enabled
    stepper_2.setPinsInverted(dir_step_2, false, false); // direction, step, enabled
}

void StepperWrapper::zeroPositions()
{
    stepper_0.setCurrentPosition(0);
    stepper_1.setCurrentPosition(0);
    stepper_2.setCurrentPosition(0);
}

void StepperWrapper::setNewStepperPositions(long *targetPositions)
{
    // Serial.printf("Stepper 1 target pos: %d\n", targetPositions[0]);

    stepper_0.moveTo(targetPositions[0]);
    stepper_1.moveTo(targetPositions[1]);
    stepper_2.moveTo(targetPositions[2]);

    // Serial.printf("Stepper 1 target pos: %d\n", stepper_0.targetPosition());
}

void StepperWrapper::initIdle()
{
    Serial.println("initIdle()");
    stepper_0.setSpeed(0);
    stepper_1.setSpeed(0);
    stepper_2.setSpeed(0);

    Serial.println(stepper_0.distanceToGo());
    Serial.println(stepper_1.distanceToGo());
    Serial.println(stepper_2.distanceToGo());
    Serial.println(stepper_0.isRunning());
    Serial.println(stepper_1.isRunning());
    Serial.println(stepper_2.isRunning());
}

void StepperWrapper::initRunning()
{
    Serial.println("initRunning()");

    stepper_0.setMaxSpeed(stepper_0_maxSpeed);
    stepper_1.setMaxSpeed(stepper_1_maxSpeed);
    stepper_2.setMaxSpeed(stepper_2_maxSpeed);
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
    stepper_0.setMaxSpeed(stepper_0_maxSpeed / 2); // half maximum speed,
    stepper_1.setMaxSpeed(stepper_1_maxSpeed / 2); // we don't need to stress
    stepper_2.setMaxSpeed(stepper_2_maxSpeed / 2); // our mechanics more than needed

    stepper_0.setAcceleration(60000);
    stepper_1.setAcceleration(40000);
    stepper_2.setAcceleration(60000);
}

void StepperWrapper::initHoming_A()
{
    Serial.println("initHoming_A()");

    // const float acceleration = 10000;

    stepper_0.setMaxSpeed(2000);
    stepper_1.setMaxSpeed(2000);
    stepper_2.setMaxSpeed(2000);

    stepper_0.setSpeed(-1600);
    stepper_1.setSpeed(-1600);
    stepper_2.setSpeed(-1600);
}

bool StepperWrapper::driveHoming_A()
{
    bool e0 = limit_switch_0.isPressed();
    bool e1 = limit_switch_1.isPressed();
    bool e2 = limit_switch_2.isPressed();
    if (!e0)
    {
        stepper_0.setSpeed(-1600);
        stepper_0.runSpeed();
    }
    if (!e1)
    {
        stepper_1.setSpeed(-1600);
        stepper_1.runSpeed();
    }
    if (!e2)
    {
        stepper_2.setSpeed(-1600);
        stepper_2.runSpeed();
    }

    return e0 && e1 && e2;
}

void StepperWrapper::initHoming_B()
{
    Serial.println("initHoming_B");
    const float acceleration = 1000;

    stepper_0.setMaxSpeed(1600);
    stepper_1.setMaxSpeed(3200);
    stepper_2.setMaxSpeed(3200);

    stepper_0.setAcceleration(acceleration);
    stepper_1.setAcceleration(acceleration);
    stepper_2.setAcceleration(acceleration);

    stepper_0.move(800);  // here: 5mm
    stepper_1.move(1600); // here: 5mm
    stepper_2.move(800);  // here: 2mm

    // stepper_0.setSpeed(800);
    // stepper_1.setSpeed(500);
    // stepper_2.setSpeed(500);
}

bool StepperWrapper::driveHoming_B()
{
    stepper_0.run();
    stepper_1.run();
    stepper_2.run();

    bool done = stepper_0.distanceToGo() == 0 &&
                stepper_1.distanceToGo() == 0 &&
                stepper_2.distanceToGo() == 0;

    return done;
}

void StepperWrapper::updateEndSwitches()
{
    limit_switch_0.update();
    limit_switch_1.update();
    limit_switch_2.update();
}

void StepperWrapper::runAllSpeedToPositions()
{
    if (limit_switch_0.isPressed() || limit_switch_1.isPressed() || limit_switch_2.isPressed())
        return;

    stepper_0.runSpeedToPosition();
    stepper_1.runSpeedToPosition();
    stepper_2.runSpeedToPosition();
}
void StepperWrapper::runAll()
{
    if (limit_switch_0.isPressed() || limit_switch_1.isPressed() || limit_switch_2.isPressed())
        return;

    stepper_0.run();
    stepper_1.run();
    stepper_2.run();
}

// bool StepperWrapper::getEndSwitch_0()
// {
//     return limit_switch_0.fell();
// }
// bool StepperWrapper::getEndSwitch_1() {}
// bool StepperWrapper::getEndSwitch_2() {}
