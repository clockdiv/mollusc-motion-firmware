#include "StepperWrapper.h"

StepperWrapper::StepperWrapper()
{

    stepper_1 = AccelStepper(AccelStepper::DRIVER, STEPPER_1_PULSE, STEPPER_1_DIR);
    stepper_2 = AccelStepper(AccelStepper::DRIVER, STEPPER_2_PULSE, STEPPER_2_DIR);
    stepper_3 = AccelStepper(AccelStepper::DRIVER, STEPPER_3_PULSE, STEPPER_3_DIR);

    stepper_1.setMinPulseWidth(minPulseWidth);
    stepper_1.setPinsInverted(false, false, false); // direction, step, enabled

    stepper_2.setMinPulseWidth(minPulseWidth);
    stepper_2.setPinsInverted(true, false, false); // direction, step, enabled

    stepper_3.setMinPulseWidth(minPulseWidth);
    stepper_3.setPinsInverted(false, false, false); // direction, step, enabled

    end_1 = Bounce2::Button();
    end_2 = Bounce2::Button();
    end_3 = Bounce2::Button();

    end_1.attach(STEPPER_1_END, INPUT_PULLUP);
    end_1.interval(25);
    end_1.setPressedState(LOW);

    end_2.attach(STEPPER_2_END, INPUT_PULLUP);
    end_2.interval(25);
    end_2.setPressedState(LOW);

    end_3.attach(STEPPER_3_END, INPUT_PULLUP);
    end_3.interval(25);
    end_3.setPressedState(LOW);

    digitalWriteFast(STEPPER_ENABLE, LOW); // enable all steppers
}

void StepperWrapper::zeroPositions()
{
    stepper_1.setCurrentPosition(0);
    stepper_2.setCurrentPosition(0);
    stepper_3.setCurrentPosition(0);
}

void StepperWrapper::setNewStepperPositions()
{
    // Serial.printf("Stepper 1 target pos: %d\n", targetPositions[0]);

    stepper_1.moveTo(targetPositions[0]);
    stepper_2.moveTo(targetPositions[1]);
    stepper_3.moveTo(targetPositions[2]);

    // Serial.printf("Stepper 1 target pos: %d\n", stepper_1.targetPosition());
}

void StepperWrapper::initIdle()
{
    Serial.println("initIdle()");
    stepper_1.setSpeed(0);
    stepper_2.setSpeed(0);
    stepper_3.setSpeed(0);

    Serial.println(stepper_1.distanceToGo());
    Serial.println(stepper_2.distanceToGo());
    Serial.println(stepper_3.distanceToGo());
    Serial.println(stepper_1.isRunning());
    Serial.println(stepper_2.isRunning());
    Serial.println(stepper_3.isRunning());
}

void StepperWrapper::initRunning()
{
    Serial.println("initRunning()");

    // const float acceleration = 10000;

    stepper_1.setMaxSpeed(stepper_1_maxSpeed);
    stepper_2.setMaxSpeed(stepper_2_maxSpeed);
    stepper_3.setMaxSpeed(stepper_3_maxSpeed);

    // stepper_1.setAcceleration(acceleration);
    // stepper_2.setAcceleration(acceleration);
    // stepper_3.setAcceleration(acceleration);
}

void StepperWrapper::initManual()
{
    // this is called when the user 'jumps' on the
    // timeline in Blender. The motors are moved with
    // acceleration to their target positions.
    Serial.println("initManual()");

    // all values for MaxSpeed and Acceleration tested
    // Stepper 1 runs with 800 MicroSteps
    // Stepper 2 runs with 1600 MicroSteps
    // Stepper 3 runs with 1600 MicroSteps
    stepper_1.setMaxSpeed(stepper_1_maxSpeed / 2); // half maximum speed,
    stepper_2.setMaxSpeed(stepper_2_maxSpeed / 2); // we don't need to stress
    stepper_3.setMaxSpeed(stepper_3_maxSpeed / 2); // our mechanics more than needed

    stepper_1.setAcceleration(60000);
    stepper_2.setAcceleration(40000);
    stepper_3.setAcceleration(60000);
}

void StepperWrapper::initHoming_A()
{
    Serial.println("initHoming_A()");

    // const float acceleration = 10000;

    stepper_1.setMaxSpeed(2000);
    stepper_2.setMaxSpeed(2000);
    stepper_3.setMaxSpeed(2000);

    // stepper_1.setAcceleration(acceleration);
    // stepper_2.setAcceleration(acceleration);
    // stepper_3.setAcceleration(acceleration);

    stepper_1.setSpeed(-1600);
    stepper_2.setSpeed(-1600);
    stepper_3.setSpeed(-1600);
}

bool StepperWrapper::driveHoming_A()
{
    bool e0 = end_1.isPressed();
    bool e1 = end_2.isPressed();
    bool e2 = end_3.isPressed();
    if (!e0)
    {
        stepper_1.setSpeed(-1600);
        stepper_1.runSpeed();
    }
    if (!e1)
    {
        stepper_2.setSpeed(-1600);
        stepper_2.runSpeed();
    }
    if (!e2)
    {
        stepper_3.setSpeed(-1600);
        stepper_3.runSpeed();
    }

    return e0 && e1 && e2;
}

void StepperWrapper::initHoming_B()
{
    Serial.println("initHoming_B");
    const float acceleration = 1000;

    stepper_1.setMaxSpeed(1600);
    stepper_2.setMaxSpeed(3200);
    stepper_3.setMaxSpeed(3200);

    stepper_1.setAcceleration(acceleration);
    stepper_2.setAcceleration(acceleration);
    stepper_3.setAcceleration(acceleration);

    stepper_1.move(800);  // here: 5mm
    stepper_2.move(1600); // here: 5mm
    stepper_3.move(800);  // here: 2mm

    // stepper_1.setSpeed(800);
    // stepper_2.setSpeed(500);
    // stepper_3.setSpeed(500);
}

bool StepperWrapper::driveHoming_B()
{
    stepper_1.run();
    stepper_2.run();
    stepper_3.run();

    bool done = stepper_1.distanceToGo() == 0 &&
                stepper_2.distanceToGo() == 0 &&
                stepper_3.distanceToGo() == 0;

    return done;
}

void StepperWrapper::updateEndSwitches()
{
    end_1.update();
    end_2.update();
    end_3.update();
}

void StepperWrapper::runAllSpeedToPositions()
{
    stepper_1.runSpeedToPosition();
    stepper_2.runSpeedToPosition();
    stepper_3.runSpeedToPosition();
}
void StepperWrapper::runAll()
{
    stepper_1.run();
    stepper_2.run();
    stepper_3.run();
}