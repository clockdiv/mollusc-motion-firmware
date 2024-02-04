#include <Arduino.h>
#include <SD.h>
#include <SPI.h>
#include <WS2812Serial.h>

#include <Bounce2.h>

#include "Logger/Logger.h"
#include "SDCardHelpers/SDCardHelpers.h"
#include "Dynamixel/Dynamixel.h"
#include "NeoPixels/NeoPixels.h"
#include "StepperWrapper/StepperWrapper.h"
#include "SerialDataHandler/SerialDataHandler.h"
#include "StateManager/StateManager.h"

#include "averageFilter.h"

#define BTN_A 16
#define BTN_B 17
#define DYNAMIXEL_COMM_DIR 22

Bounce2::Button btn_a;
Bounce2::Button btn_b;

Logger logger;
SDCardHelpers sdCardHelpers;
Dynamixel2Arduino dxl(Serial5, DYNAMIXEL_COMM_DIR);
Dynamixel dynamixel(dxl);
NeoPixels neoPixels;
StepperWrapper stepperWrapper;
SerialDataHandler serialDataHandler;
StateManager stateManager;

unsigned long current_millis, previous_millis;

void setup()
{
  Serial.begin(500000);

  delay(100);

  sdCardHelpers.initSD();
  sdCardHelpers.printAllFiles();

  delay(100);

  // log_sd("============================================================");
  // log_sd("starting board");
  // log_sd("============================================================");

  // analogReadResolution(13);

  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(STEPPER_ENABLE, OUTPUT);
  digitalWriteFast(LED_BUILTIN, LOW);

  btn_a = Bounce2::Button();
  btn_b = Bounce2::Button();
  btn_a.attach(BTN_A, INPUT_PULLUP);
  btn_a.interval(25);
  btn_a.setPressedState(LOW);
  btn_b.attach(BTN_B, INPUT_PULLUP);
  btn_b.interval(25);
  btn_b.setPressedState(LOW);

  // end_1.attach(STEPPER_1_END, INPUT_PULLUP);
  // end_1.interval(25);
  // end_1.setPressedState(LOW);

  // end_2.attach(STEPPER_2_END, INPUT_PULLUP);
  // end_2.interval(25);
  // end_2.setPressedState(LOW);

  // end_3.attach(STEPPER_3_END, INPUT_PULLUP);
  // end_3.interval(25);
  // end_3.setPressedState(LOW);

  dynamixel.init_dxl();
  neoPixels.init();

  // for (int i = 0; i < 1024; i++)
  // {
  //   poti_a = poti_a_filtered.filter(analogRead(POTI_A));
  //   poti_b = poti_b_filtered.filter(analogRead(POTI_B));
  // }
  // poti_a_old = poti_a;
  // poti_b_old = poti_b;

  stateManager.setState(IDLE);

  // log_sd("setup done, starting mainloop");

  previous_millis = millis();
}

void loop()
{
  current_millis = millis();

  btn_a.update();
  btn_b.update();
  stepperWrapper.updateEndSwitches();

  if (serialDataHandler.receiveAsCSV())
  {
    switch (serialDataHandler.serialData.command)
    {
    case SerialCommand::STATE_CHANGE:
      // setState(serialDataHandler.serialData.stateAsString); TODO
      break;

    case SerialCommand::POSITION_DATA:
      stepperWrapper.setNewStepperPositions(serialDataHandler.serialData.targetPositionsSteppers);
      dynamixel.setNewDynamixelPositions(serialDataHandler.serialData.targetPositionsServos);
      //     if (state == RUNNING)
      //     {
      //         // TODO enable again!
      //         float speed_stepper_1 = stepper_1_speed_filtered.filter(fps * float(stepper_1.distanceToGo()) * 0.1f);
      //         stepper_1.setSpeed(speed_stepper_1);
      //         // TODO enable again!
      //         float speed_stepper_2 = stepper_2_speed_filtered.filter(fps * float(stepper_2.distanceToGo()) * 0.1f);
      //         stepper_2.setSpeed(speed_stepper_2);
      //         // TODO enable again!
      //         float speed_stepper_3 = stepper_3_speed_filtered.filter(fps * float(stepper_3.distanceToGo()) * 0.1f);
      //         stepper_3.setSpeed(speed_stepper_3);
      //     }

      break;

    default:
      break;
    }
  }

  // fade neopixels
  float neopixel_brightness = (sin(millis() / 500.0) + 1.0) / 2.0 * 55.0;
  neoPixels.set_all(WHITE, int(neopixel_brightness));

  if (current_millis - previous_millis > 100)
  {
    // Serial.print(current_millis);
    // Serial.print(": ");
    // Serial.print(btn_a.isPressed());
    // Serial.print(" - ");
    // Serial.print(btn_b.isPressed());
    // Serial.print(" - ");
    // Serial.print(end_1.isPressed());
    // Serial.print(" - ");
    // Serial.print(end_2.isPressed());
    // Serial.print(" - ");
    // Serial.println(end_3.isPressed());
    previous_millis = current_millis;
  }
  if (btn_a.fell())
  {
    if (stateManager.getState() == HOMING_A || stateManager.getState() == HOMING_B)
    {
      stateManager.setState(States::IDLE);
    }
    else
    {
      stateManager.setState(States::HOMING_A);
    }
  }

  if (btn_b.fell())
  {
    Serial.println("Rebooting Dynamixels");
    dynamixel.disableTorque();
    dynamixel.enableLEDs();
    delay(2000);
    dynamixel.rebootDynamixels();
    dynamixel.disableLEDs();
    dynamixel.enableTorque();

    delay(300);
    dynamixel.enableLEDs();
    delay(300);
    dynamixel.disableLEDs();
  }

  switch (stateManager.getState())
  {
  case States::IDLE:
    serialDataHandler.receiveAsCSV();
    break;
  case States::RUNNING:
    serialDataHandler.receiveAsCSV();
    /* runSpeed(): Poll the motor and step it if a step is due, implementing a constant
                   speed as set by the most recent call to setSpeed().
                   You must call this as frequently as possible, but at least once per step interval.

       runSpeedToPosition(): Executes runSpeed() unless the targetPosition is reached. This function
                             needs to be called often just like runSpeed() or run().
                             Will step the motor if a step is required at the currently selected speed
                             unless the target position has been reached. Does not implement accelerations. */
    // stepper_1.runSpeedToPosition();
    // stepper_2.runSpeedToPosition();
    // stepper_3.runSpeedToPosition();
    stepperWrapper.runAllSpeedToPositions();
    break;
  case States::MANUAL:
    serialDataHandler.receiveAsCSV();
    /* run(): Poll the motor and step it if a step is due, implementing accelerations and decelerations to achieve the target position.
       You must call this as frequently as possible, but at least once per minimum step time interval, preferably in your main loop.
       Note that each call to run() will make at most one step, and then only when a step is due, based on the current speed and the time since the last step. */

    // stepper_1.run();
    // stepper_2.run();
    // stepper_3.run();
    stepperWrapper.runAll();
    break;
  case States::HOMING_A:
    if (stepperWrapper.driveHoming_A())
    {
      stateManager.setState(States::HOMING_B);
    }
    break;
  case States::HOMING_B:
    if (stepperWrapper.driveHoming_B())
    {
      stepperWrapper.zeroPositions();
      stateManager.setState(States::IDLE);
    }
    break;
  default:
    break;
  }

  // Enable LED_BUILTIN when a stepper is in running
  // TODO Enable again:
  // digitalWriteFast(LED_BUILTIN, stepper_1.isRunning() || stepper_2.isRunning() || stepper_3.isRunning());

  // handle_current_state();
}
