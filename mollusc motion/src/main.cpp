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
#include "TimeWrapper/TimeWrapper.h"
#include "PSRamHandler/PSRamHandler.h"

#include "averageFilter.h"

#define BTN_A 17 // PCB v1.1
#define BTN_B 14 // PCB v1.1
// #define BTN_A 16 // PCB v1.2
// #define BTN_B 17 // PCB v1.2
#define DYNAMIXEL_COMM_DIR 22

#define HARDWARETEST true

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
TimeWrapper timeWrapper;

unsigned long current_millis, previous_millis;

void HardwareTest();

void setup()
{
  Serial.begin(500000);

  delay(100);

  pinMode(LED_BUILTIN, OUTPUT);
  digitalWriteFast(LED_BUILTIN, LOW);

  btn_a = Bounce2::Button();
  btn_b = Bounce2::Button();
  btn_a.attach(BTN_A, INPUT_PULLUP);
  btn_a.interval(25);
  btn_a.setPressedState(LOW);
  btn_b.attach(BTN_B, INPUT_PULLUP);
  btn_b.interval(25);
  btn_b.setPressedState(LOW);

  delay(100);

  if (HARDWARETEST || btn_a.isPressed() || btn_b.isPressed())
  {
    HardwareTest();
    Serial.printf("\n\n");
    Serial.println(F("Test finished."));
    // Stop after HardwareTest
    while (true)
    {
    }
  }

  sdCardHelpers.initSD();

  delay(100);

  // log_sd("============================================================");
  // log_sd("starting board");
  // log_sd("============================================================");

  // analogReadResolution(13);

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
    case SerialCommand::SET_TIME:
      timeWrapper.setCurrentTime(serialDataHandler.serialData.pctime);
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

void HardwareTest()
{
  const unsigned long timeout = 10000;
  while (!Serial)
    ;

  elapsedMillis timer1sec, timer10sec;

  digitalWriteFast(LED_BUILTIN, HIGH);
  Serial.println(F("Entered Testmode"));
  Serial.println(F("================"));
  Serial.println();
  Serial.println(F("Test 1 - Buttons"));
  Serial.println(F("----------------"));

  // Test Button A
  // =============
  Serial.println(F("Press button A"));
  timer10sec = 0;
  timer1sec = 0;
  while (true)
  {
    btn_a.update();
    if (timer1sec >= 1000)
    {
      timer1sec = timer1sec - 1000;
      Serial.print(String(10 - timer10sec / 1000) + "... ");
    }
    if (btn_a.fell())
    {
      Serial.println(F("Button A working"));
      break;
    }
    if (timer10sec >= timeout - 1)
    {
      Serial.println(F("Timeout"));
      break;
    }
  }

  // Test Button B
  // =============
  Serial.println(F("Press button B"));
  timer10sec = 0;
  timer1sec = 0;
  while (true)
  {
    btn_b.update();
    if (timer1sec >= 1000)
    {
      timer1sec = timer1sec - 1000;
      Serial.print(String(10 - timer10sec / 1000) + "... ");
    }
    if (btn_b.fell())
    {
      Serial.println(F("Button B working"));
      break;
    }
    if (timer10sec >= timeout)
    {
      Serial.println(F("Timeout"));
      break;
    }
  }

  // Test RealTimeClock
  // ==================
  Serial.println(F("\nTest 2 - RealTimeClock"));
  Serial.println(F("----------------------"));
  Serial.print(F("Set time by sending letter 'T' followed by ten digit time (as seconds since Jan 1 1970), via serial, "));
  Serial.println(F("e.g. T1357041600 for January 1st 2013"));
  timer10sec = 0;
  timer1sec = 0;

  while (timer10sec <= timeout - 1)
  {
    if (timer1sec >= 1000)
    {
      timer1sec = timer1sec - 1000;
      Serial.print(String(10 - timer10sec / 1000) + "... ");
      Serial.println(timeWrapper.currentTimeAsString());
    }
    if (serialDataHandler.receiveAsCSV())
    {
      if (serialDataHandler.serialData.command == SerialCommand::SET_TIME)
      {
        TimeWrapper::setCurrentTime(serialDataHandler.serialData.pctime);
      }
    }
  }

  // Additional Memory
  // =========================
  Serial.println(F("\nTest 3 - PSRAM"));
  Serial.println(F("--------------"));
  Serial.println(F("(additional 8 pin QSPI 8MB memory chip soldered on the backside of the Teensy.)"));
  if (PSRamHandler::runTest())
  {
    Serial.printf(F("Test passed\n"));
  }
  else
  {
    Serial.printf(F("Test failed\n"));
  }

  // Test SD-Card
  // =============
  Serial.println(F("\nTest 2 - SD-Card"));
  Serial.println(F("----------------"));
  if (sdCardHelpers.initSD())
  {
    timer10sec = 0;
    timer1sec = 0;
    String filename, message;
    Serial.println(F("Files on SD-Card:"));
    sdCardHelpers.printAllFiles();
    Serial.println();
    while (timer10sec <= timeout - 1)
    {
      if (timer1sec >= 1000)
      {
        Serial.print(String(10 - timer10sec / 1000) + "... ");
        if (TimeWrapper::checkStatus())
        {
          filename = TimeWrapper::dateFormattedAsFilePrefix() + ".TXT";
          message = TimeWrapper::currentTimeAsString() + " This is a hardware test.";
        }
        else
        {
          filename = "TEST.TXT";
          message = String(timer10sec) + " This is a hardware test. RTC not set.";
        }
        if (SDCardHelpers::writeTextFile(filename, message))
        {
          Serial.printf("Wrote to file '%s': %s\n", filename.c_str(), message.c_str());
        }
        else
        {
          Serial.printf("Failed writing file '%s' to SD-Card\n", filename.c_str());
        }
        timer1sec = timer1sec - 1000;
      }
    }
    Serial.println(F("Files on SD-Card:"));
    sdCardHelpers.printAllFiles();
    Serial.println();
  }

  // Test Network
  // =============
  Serial.println(F("\nTest 3 - Network connection"));
  Serial.println(F("---------------------------"));
  timer10sec = 0;
  timer1sec = 0;

  while (timer10sec <= timeout - 1)
  {
    if (timer1sec >= 1000)
    {
      timer1sec = timer1sec - 1000;
      Serial.print(String(10 - timer10sec / 1000) + "... ");
    }
    // ToDo: Network Test
    // Show IP
    // try connect to 8.8.8.8
    // wait 10 sec for OSC messages...
    // ...and broadcast a OSC message every 1 seconds
  }
  Serial.println();

  // Test NeoPixels
  // ==============
  Serial.println(F("\nTest 2 - NeoPixels"));
  Serial.println(F("------------------"));
  Serial.println(F("All NeoPixels should fade WHITE for 10 seconds."));
  neoPixels.init();
  timer10sec = 0;
  timer1sec = 0;

  while (timer10sec <= timeout - 1)
  {
    if (timer1sec >= 1000)
    {
      timer1sec = timer1sec - 1000;
      Serial.print(String(10 - timer10sec / 1000) + "... ");
    }

    float neopixel_brightness = (sin(millis() / 500.0) + 1.0) / 2.0 * 55.0;
    neoPixels.set_all(WHITE, int(neopixel_brightness));
  }
  neoPixels.set_all(WHITE, 0);
  Serial.println();

  // Test Dynamixel
  // ==============
  Serial.println(F("\nTest 4 - Dynamixels"));
  Serial.println(F("-------------------"));
  Serial.println(F("All Dynamixels should move a bit."));

  dynamixel.init_dxl();

  int32_t goal_position[] = {2048, 2048 - 256, 2048, 2048 + 256};
  int goal_position_index = 0;
  long targetPositionsServos[16];
  timer10sec = 0;
  timer1sec = 0;
  Serial.print(F("Number of Servos addressed: "));
  Serial.println(sizeof(targetPositionsServos) / sizeof(long));
  while (timer10sec <= timeout - 1)
  {
    if (timer1sec >= 1000)
    {
      timer1sec = timer1sec - 1000;
      Serial.print(String(10 - timer10sec / 1000) + "... ");
      for (u_int i = 0; i < sizeof(targetPositionsServos) / sizeof(long); i++)
      {
        targetPositionsServos[i] = goal_position[goal_position_index % 4];
      }
      Serial.print(F("Moving servos to:\n"));
      for (int i = 0; i < 16; i++)
      {
        Serial.printf("ID: %d, Position: %lu\n", i, targetPositionsServos[i]);
      }
      Serial.println();
      dynamixel.setNewDynamixelPositions(targetPositionsServos);
      goal_position_index++;
    }
  }
  // ToDo: Read present positions, read temperature
  Serial.println();

  // Test Stepper Motors
  // ===================
  Serial.println(F("\nTest 5 - Stepper Motors"));
  Serial.println(F("-----------------------"));
  Serial.println(F("All Steppers should move a bit forward and backwards."));
  timer10sec = 0;
  timer1sec = 0;
  while (timer10sec <= timeout - 1)
  {
    if (timer1sec >= 1000)
    {
      timer1sec = timer1sec - 1000;
      Serial.print(String(10 - timer10sec / 1000) + "... ");
    }
    // ToDo: Stepper Movement
  }
  Serial.println();

  // Test Stepper End Switches
  // =========================
  Serial.println(F("\nTest 6 - Stepper End Switches"));
  Serial.println(F("-----------------------------"));
  Serial.println(F("All Steppers move slowly till the end switches are reached."));
  timer10sec = 0;
  timer1sec = 0;
  while (timer10sec <= timeout - 1)
  {
    if (timer1sec >= 1000)
    {
      timer1sec = timer1sec - 1000;
      Serial.print(String(10 - timer10sec / 1000) + "... ");
    }
    // ToDo: Stepper Movement and End-Switch Test
  }
  Serial.println();

  // Temperature
  // ===========
  Serial.println(F("\nTest 7 - Temperature"));
  Serial.println(F("-----------------------------"));
  timer10sec = 0;
  timer1sec = 0;
  tempmon_init();
  tempmon_Start();
  while (timer10sec <= timeout - 1)
  {
    if (timer1sec >= 1000)
    {
      timer1sec = timer1sec - 1000;
      Serial.print(String(10 - timer10sec / 1000) + "... ");
      Serial.print("CPU Temperature: ");
      Serial.println(tempmonGetTemp());
      Serial.printf("%cC\n", 167); // 167 = ASCII for °
      Serial.printf("%cC\n", 176); // 167 = ASCII for °
      Serial.println(char(176));
      Serial.println(char(167));
      Serial.println("°");
    }
  }
  tempmon_Stop();
  tempmon_PwrDwn();
}