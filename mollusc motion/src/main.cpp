#include <Arduino.h>
#include <SD.h>
#include <SPI.h>
#include <WS2812Serial.h>
// #include <iostream>
// #include <stdio.h>
#include <map>

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
#include "NetworkHandler/NetworkHandler.h"

#include "averageFilter.h"

#define HARDWARETEST false
const char filename[] = "musk0304.bin";

#define BTN_A 16 // PCB v1.2
#define BTN_B 17 // PCB v1.2
#define DYNAMIXEL_COMM_DIR 22

Bounce2::Button btn_a;
Bounce2::Button btn_b;

Logger logger;
Dynamixel2Arduino dxl(Serial5, DYNAMIXEL_COMM_DIR);
Dynamixel dynamixel(dxl);
NeoPixels neoPixels;
StepperWrapper stepperWrapper;
SerialDataHandler serialDataHandler;
TimeWrapper timeWrapper;

elapsedMillis ledBlinkTimer = 0;
const uint16_t ledBlinkIntervalIdleState = 1000;
const uint16_t ledBlinkIntervalTestState = 250;
const uint16_t ledBlinkIntervalErrorState = 50;
bool LED_BUILTIN_STATE = LOW;

elapsedMillis waitForSerialTimeout = 0;

elapsedMillis debugMessageTimer = 0;
const unsigned long debugMessageInterval = 1000;

elapsedMicros playTimer = 0;
const long fps = 60;
const long playFrameInterval = 1000000 / fps;

uint32_t startFrame = 0;
uint32_t endFrame = 66199;
uint32_t currentFrame = 0;
uint16_t frameData[14];

/// @brief Sets the board into error mode which can be skipped with the button A.
/// @param errorMsg The error message to be logged.
void error(String errorMsg)
{
  Serial.print("STARTUP ERROR: ");
  Serial.println(errorMsg);
  Serial.println("Press A to continue.");
  while (true)
  {
    if (ledBlinkTimer > ledBlinkIntervalErrorState)
    {
      LED_BUILTIN_STATE = !LED_BUILTIN_STATE;
      digitalWriteFast(LED_BUILTIN, LED_BUILTIN_STATE);
      ledBlinkTimer = 0;
    }

    btn_a.update();
    if (btn_a.fell())
    {
      return;
    }
  }
}

/// @brief Switches into PLAY state and sets the needed variables.
/// @param filename
/// @param startFrame
/// @param endFrame
void playCallback(/*String filename, uint32_t startFrame, uint32_t endFrame*/)
{
  Serial.println("PLAY CALLBACK");
  // Serial.println(filename);
  // Serial.println(startFrame);
  // Serial.println(endFrame);
  StateManager::setState(States::PLAYING);
}

/// @brief Switches into IDLE state.
void pauseCallback()
{
  Serial.println("PAUSE CALLBACK");
  StateManager::setState(States::IDLE);
}

/// @brief Switches into HOME state.
void homeCallback()
{
  if (StateManager::getState() == States::IDLE)
  {
    Serial.println("HOME CALLBACK");
    dynamixel.rebootDynamixels();
    dynamixel.disableTorque();
    dynamixel.enableLEDs();

    StateManager::setState(States::HOMING_A);
    currentFrame = 0;
  }
  else
  {
    Serial.print("Can't switch to HOME because I'm not IDLE.");
  }
}

bool hardwaretestButtonA()
{
  btn_a.update();
  return btn_a.fell();
}

bool hardwaretestButtonB()
{
  btn_b.update();
  return btn_b.fell();
}

bool hardwaretestInitMotors()
{
  stepperWrapper.stepper[0].setPinsInverted(false, false, false); // direction, step, enabled
  stepperWrapper.stepper[1].setPinsInverted(true, true, false);   // direction, step, enabled
  stepperWrapper.stepper[2].setPinsInverted(true, true, false);   // direction, step, enabled
  stepperWrapper.stepper[3].setPinsInverted(false, true, false);  // direction, step, enabled

  stepperWrapper.stepper[0].setCurrentPosition(0);
  stepperWrapper.stepper[0].setMaxSpeed(100);
  stepperWrapper.stepper[0].setAcceleration(20);
  stepperWrapper.stepper[0].moveTo(500);

  stepperWrapper.stepper[1].setCurrentPosition(0);
  stepperWrapper.stepper[1].setMaxSpeed(500);
  stepperWrapper.stepper[1].setAcceleration(100);
  stepperWrapper.stepper[1].moveTo(1500);

  stepperWrapper.stepper[2].setCurrentPosition(0);
  stepperWrapper.stepper[2].setMaxSpeed(450);
  stepperWrapper.stepper[2].setAcceleration(100);
  stepperWrapper.stepper[2].moveTo(400);

  stepperWrapper.stepper[3].setCurrentPosition(0);
  stepperWrapper.stepper[3].setMaxSpeed(550);
  stepperWrapper.stepper[3].setAcceleration(120);
  stepperWrapper.stepper[3].moveTo(600);

  return true;
}

bool hardwaretestDriveMotors()
{
  // If at the end of travel go to the other end
  for (uint8_t i = 0; i < stepperWrapper.stepperCount; i++)
  {
    if (stepperWrapper.stepper[i].distanceToGo() == 0)
      stepperWrapper.stepper[i].moveTo(-stepperWrapper.stepper[i].currentPosition());

    stepperWrapper.stepper[i].run();
  }

  return false;
}
/// @brief Runs 11 test routines for the hardware.
void HardwareTest()
{
  const unsigned long timeout = 10000;
  elapsedMillis timer1sec, timer10sec;

  ledBlinkTimer = 0;
  while (!Serial)
  {
    if (ledBlinkTimer > ledBlinkIntervalTestState)
    {
      LED_BUILTIN_STATE = !LED_BUILTIN_STATE;
      digitalWriteFast(LED_BUILTIN, LED_BUILTIN_STATE);
      ledBlinkTimer = 0;
    }
  }

  digitalWriteFast(LED_BUILTIN, HIGH);
  Serial.println(F("Entered Testmode"));
  Serial.println(F("================"));

  // Map functions with a name; the order is specified below.
  std::map<bool (*)(), String> hardwaretestWithNames;
  hardwaretestWithNames[hardwaretestButtonA] = "Button A (Please press Button A)";
  hardwaretestWithNames[hardwaretestButtonB] = "Button B (Please press Button A)";
  hardwaretestWithNames[hardwaretestDriveMotors] = "Driving Motors (press any Button to end)";

  // in this array we define the order of the tests
  bool (*hardwaretest[])() = {

      hardwaretestWithNames.find(hardwaretestDriveMotors)->first,
      hardwaretestWithNames.find(hardwaretestButtonA)->first,
      hardwaretestWithNames.find(hardwaretestButtonB)->first,
  };
  int hardwaretestCount = sizeof(hardwaretest) / sizeof(bool *);

  hardwaretestInitMotors();

  for (int i = 0; i < hardwaretestCount; i++)
  {
    String testmethodName = hardwaretestWithNames.find(hardwaretest[i])->second;
    Serial.printf("\n___%d/%d \"%s\" ", i + 1, hardwaretestCount, testmethodName.c_str());

    unsigned long testTimeout = 1000;
    bool testResult = false;
    for (uint8_t j = 10; j > 0; j--)
    {
      elapsedMillis testTimer = 0;
      Serial.printf("%d..", j);
      while (testTimer < testTimeout && !testResult)
      {
        testResult = hardwaretest[i]();
      }
      if (testResult)
        break;
    }
    String result = "";
    testResult == true ? result = "ok" : result = "FAILED";
    Serial.printf(" %s", result.c_str());
  }

  return;

  // Test RealTimeClock
  // ==================
  Serial.println(F("\nTest 3/11 - RealTimeClock"));
  Serial.println(F("-------------------------"));
  Serial.print(F("Set time by sending letter 'T' followed by ten digit time (as seconds since Jan 1 1970), via serial, "));
  Serial.println(F("e.g. T1357041600 for January 1st 2013"));
  timer10sec = 0;
  timer1sec = 0;

  while (timer10sec <= timeout - 1)
  {
    if (timer1sec >= 1000)
    {
      timer1sec = timer1sec - 1000;
      Serial.print(String((timeout - timer10sec) / 1000) + "... ");
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
  Serial.println(F("\nTest 4/11 - PSRAM"));
  Serial.println(F("-----------------"));
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
  Serial.println(F("\nTest 5/11 - SD-Card"));
  Serial.println(F("-------------------"));
  if (SDCardHelpers::initSD())
  {
    timer10sec = 0;
    timer1sec = 0;
    String filename, message;
    Serial.println(F("Files on SD-Card:"));
    SDCardHelpers::printAllFiles();
    Serial.println();
    while (timer10sec <= timeout - 1)
    {
      if (timer1sec >= 1000)
      {
        Serial.print(String((timeout - timer10sec) / 1000) + "... ");
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
    SDCardHelpers::printAllFiles();
    Serial.println();
  }

  // Test Network
  // =============
  Serial.println(F("\nTest 6/11 - Network connection"));
  Serial.println(F("------------------------------"));
  timer10sec = 0;
  timer1sec = 0;
  bool network_ok = false;
  if (!NetworkHandler::init())
  {
    Serial.println("Error initializing network handler");
  }
  if (NetworkHandler::HardwareStatus())
  {
    Serial.println(F("Hardware found"));
    if (NetworkHandler::CableConnected())
    {
      network_ok = true;
      Serial.println(F("Cable connected"));
      Serial.print(F("Local IP Address: "));
      Serial.println(NetworkHandler::GetIp());
    }
    else
    {
      Serial.println(F("No ethernet cable connected"));
    }
  }
  else
  {
    Serial.println(F("No Ethernet Hardware found"));
  }

  if (network_ok)
  {
    int counter = 0;
    while (timer10sec <= timeout - 1)
    {
      NetworkHandler::updateIncomingOSC();

      if (timer1sec >= 1000)
      {
        timer1sec = timer1sec - 1000;
        Serial.print(String((timeout - timer10sec) / 1000) + "... ");

        String osc_message = "Hello from mollusc motion #" + String(counter);
        NetworkHandler::sendOSCString(osc_message);
        counter++;
      }
    }
  }
  Serial.println();

  // Test NeoPixels
  // ==============
  Serial.println(F("\nTest 7/11 - NeoPixels"));
  Serial.println(F("---------------------"));
  Serial.println(F("All NeoPixels should fade WHITE for 10 seconds."));
  neoPixels.init();
  timer10sec = 0;
  timer1sec = 0;

  while (timer10sec <= timeout - 1)
  {
    if (timer1sec >= 1000)
    {
      timer1sec = timer1sec - 1000;
      Serial.print(String((timeout - timer10sec) / 1000) + "... ");
    }

    float neopixel_brightness = (sin(millis() / 500.0) + 1.0) / 2.0 * 55.0;
    neoPixels.set_all(WHITE, int(neopixel_brightness));
  }
  neoPixels.set_all(WHITE, 0);
  Serial.println();

  // Test Dynamixel
  // ==============
  Serial.println(F("\nTest 8/11 - Dynamixels"));
  Serial.println(F("----------------------"));
  Serial.println(F("All Dynamixels should move a bit."));

  dynamixel.init_dxl();

  const int32_t center = 2048;
  const int32_t offset = 256;
  const int32_t goal_position[] = {center,
                                   center - offset,
                                   center,
                                   center + offset};
  int goal_position_index = 0;
  long targetPositionsServos[16];
  u_int targetPositionsCount = sizeof(targetPositionsServos) / sizeof(long);
  timer10sec = 0;
  timer1sec = 0;
  Serial.print(F("Number of Servos addressed: "));
  Serial.println(targetPositionsCount);
  while (timer10sec <= timeout - 1)
  {
    if (timer1sec >= 1000)
    {
      timer1sec = timer1sec - 1000;
      Serial.print(String((timeout - timer10sec) / 1000) + "... ");
      for (u_int i = 0; i < targetPositionsCount; i++)
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
  Serial.println();

  // Test Stepper End Switch 0
  // =========================
  Serial.println(F("\nTest 9/11 - Stepper End Switches"));
  Serial.println(F("--------------------------------"));
  Serial.println(F("Please activate all three endswitches manually."));
  timer10sec = 0;
  timer1sec = 0;
  while (timer10sec <= timeout - 1)
  {
    stepperWrapper.updateEndSwitches();
    if (timer1sec >= 1000)
    {
      timer1sec = timer1sec - 1000;
      Serial.print(String((timeout - timer10sec) / 1000) + "... ");
    }

    for (uint8_t i = 0; i < stepperWrapper.stepperCount; i++)
    {
      if (stepperWrapper.limitSwitch[i].fell())
      {
        Serial.printf("\nLimit switch %d pressed\n", i);
        timer10sec = 0;
      }
      else if (stepperWrapper.limitSwitch[i].rose())
      {
        Serial.printf("\nLimit switch %d released\n", i);
        timer10sec = 0;
      }
    }
    // if (stepperWrapper.limit_switch_0.fell())
    // {
    //   Serial.println(F("\nLimit switch 0 pressed"));
    //   timer10sec = 0;
    // }
    // else if (stepperWrapper.limit_switch_0.rose())
    // {
    //   Serial.println(F("\nLimit switch 0 released"));
    //   timer10sec = 0;
    // }

    // if (stepperWrapper.limit_switch_1.fell())
    // {
    //   Serial.println(F("\nLimit switch 1 pressed"));
    //   timer10sec = 0;
    // }
    // else if (stepperWrapper.limit_switch_1.rose())
    // {
    //   Serial.println(F("\nLimit switch 1 released"));
    //   timer10sec = 0;
    // }

    // if (stepperWrapper.limit_switch_2.fell())
    // {
    //   Serial.println(F("\nLimit switch 2 pressed"));
    //   timer10sec = 0;
    // }
    // else if (stepperWrapper.limit_switch_2.rose())
    // {
    //   Serial.println(F("\nLimit switch 2 released"));
    //   timer10sec = 0;
    // }
  }
  Serial.println();

  // Test Stepper Motors
  // ===================
  Serial.println(F("\nTest 10/11 - Stepper Motors"));
  Serial.println(F("--------------------------"));
  Serial.println(F("All Steppers move 15000 steps forward and backwards."));
  timer10sec = 0;
  timer1sec = 0;
  stepperWrapper.setDirPins(false, false, false, false);

  long targetPositionsSteppers[] = {15000, 15000, 15000};
  stepperWrapper.initManual();
  stepperWrapper.setNewStepperPositions(targetPositionsSteppers);

  while (timer10sec <= timeout - 1)
  {
    if (timer1sec >= 1000)
    {
      timer1sec = timer1sec - 1000;
      Serial.print(String((timeout - timer10sec) / 1000) + "... ");
    }
    stepperWrapper.runAll();
  }

  targetPositionsSteppers[0] = 0;
  targetPositionsSteppers[1] = 0;
  targetPositionsSteppers[2] = 0;
  stepperWrapper.setNewStepperPositions(targetPositionsSteppers);
  timer10sec = 0;
  timer1sec = 0;

  while (timer10sec <= timeout - 1)
  {
    if (timer1sec >= 1000)
    {
      timer1sec = timer1sec - 1000;
      Serial.print(String((timeout - timer10sec) / 1000) + "... ");
    }
    stepperWrapper.runAll();
  }
  Serial.println();

  // Temperature
  // ===========
  Serial.println(F("\nTest 11/11 - CPU Temperature"));
  Serial.println(F("---------------------------"));
  timer10sec = 0;
  timer1sec = 0;
  tempmon_init();
  tempmon_Start();
  while (timer10sec <= timeout - 1)
  {
    if (timer1sec >= 1000)
    {
      timer1sec = timer1sec - 1000;
      Serial.print(String((timeout - timer10sec) / 1000) + "... ");
      Serial.print("CPU Temperature: ");
      Serial.print(tempmonGetTemp());
      Serial.printf(" Celsius\n");
    }
  }
  tempmon_Stop();
  tempmon_PwrDwn();

  digitalWriteFast(LED_BUILTIN, LOW);
}

void setup()
{
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWriteFast(LED_BUILTIN, HIGH);

  Serial.begin(500000);

  delay(100);
  btn_a = Bounce2::Button();
  btn_b = Bounce2::Button();
  btn_a.attach(BTN_A, INPUT_PULLUP);
  btn_a.interval(25);
  btn_a.setPressedState(LOW);
  btn_b.attach(BTN_B, INPUT_PULLUP);
  btn_b.interval(25);
  btn_b.setPressedState(LOW);

  delay(100);

  btn_a.update();
  btn_b.update();

  if (HARDWARETEST /* || btn_a.isPressed() || btn_b.isPressed()*/)
  {
    HardwareTest();
    Serial.printf("\n\n");
    Serial.println(F("Test finished."));
    // Stop after HardwareTest
    while (true)
    {
    }
  }

  /*

  waitForSerialTimeout = 0;
  while (!Serial && (waitForSerialTimeout < 5000))
  {
  }

  Serial.println(F("Welcome to molluscmotion."));

  Serial.println(F("Initializing SD-Card."));
  if (!SDCardHelpers::initSD())
  {
    error("Failed initializing sd card");
  }
  else
  {
    Serial.print(F("Loading file: "));
    Serial.println(filename);
    if (!SDCardHelpers::loadFileToExtmem(filename))
    {
      error("Failed loading file to extmem.");
    }
  }

  delay(100);

  Serial.println(F("Initializing network."));
  if (!NetworkHandler::init())
  {
    error("Failed initializing network");
  }
  else
  {
    Serial.print(F("local IP: "));
    Serial.println(NetworkHandler::GetIp());
    NetworkHandler::registerPlayCallback(playCallback);
    NetworkHandler::registerPauseCallback(pauseCallback);
    NetworkHandler::registerHomeCallback(homeCallback);
  }

  */

  delay(100);

  dynamixel.init_dxl();

  // stepperWrapper.setDirPins(false, false, false, false);

  stepperWrapper.stepper[0].setPinsInverted(true, false, false); // direction, step, enabled
  stepperWrapper.stepper[1].setPinsInverted(false, true, false); // direction, step, enabled
  stepperWrapper.stepper[2].setPinsInverted(true, true, false);  // direction, step, enabled
  stepperWrapper.stepper[3].setPinsInverted(false, true, false); // direction, step, enabled

  stepperWrapper.stepper[0].setMaxSpeed(100);
  stepperWrapper.stepper[1].setMaxSpeed(500);
  stepperWrapper.stepper[2].setMaxSpeed(450);
  stepperWrapper.stepper[3].setMaxSpeed(550);

  stepperWrapper.stepper[0].setAcceleration(20);
  stepperWrapper.stepper[1].setAcceleration(100);
  stepperWrapper.stepper[2].setAcceleration(100);
  stepperWrapper.stepper[3].setAcceleration(120);

  stepperWrapper.stepper[0].setCurrentPosition(10000); // Body
  stepperWrapper.stepper[1].setCurrentPosition(10000); // Neck
  stepperWrapper.stepper[2].setCurrentPosition(0);     // Head Left
  stepperWrapper.stepper[3].setCurrentPosition(0);     // Head Right

  neoPixels.init();

  Serial.println(F("Initialization done, running."));
  StateManager::setStepperWrapper(&stepperWrapper);
  StateManager::setState(IDLE);
}

void loop()
{

  btn_a.update();
  btn_b.update();
  stepperWrapper.updateEndSwitches();

  if (serialDataHandler.receiveAsCSV())
  {
    switch (serialDataHandler.serialData.command)
    {
    case SerialCommand::STATE_CHANGE:
      StateManager::setStateFromString(String(serialDataHandler.serialData.stateAsString));
      break;
    case SerialCommand::SET_TIME:
      timeWrapper.setCurrentTime(serialDataHandler.serialData.pctime);
      break;
    case SerialCommand::POSITION_DATA:
      stepperWrapper.setNewStepperPositions(serialDataHandler.serialData.targetPositionsSteppers);
      // if (String(serialDataHandler.serialData.stateAsString) == "RUNNING")
      // {
      //   StateManager::setState(States::RUNNING);
      // }
      // else if (String(serialDataHandler.serialData.stateAsString) == "MANUAL")
      // {
      //   StateManager::setState(States::MANUAL);
      // }

      // dynamixel.setNewDynamixelPositions(serialDataHandler.serialData.targetPositionsServos);
      //     if (state == RUNNING)
      //     {
      //         // TODO enable again!
      //         float speed_stepper_1 = stepper_0_speed_filtered.filter(fps * float(stepper_0.distanceToGo()) * 0.1f);
      //         stepper_0.setSpeed(speed_stepper_1);
      //         // TODO enable again!
      //         float speed_stepper_2 = stepper_1_speed_filtered.filter(fps * float(stepper_1.distanceToGo()) * 0.1f);
      //         stepper_1.setSpeed(speed_stepper_2);
      //         // TODO enable again!
      //         float speed_stepper_3 = stepper_2_speed_filtered.filter(fps * float(stepper_2.distanceToGo()) * 0.1f);
      //         stepper_2.setSpeed(speed_stepper_3);
      //     }
      break;
    default:
      break;
    }
  }

  if (btn_a.fell())
  {
    if (StateManager::getState() == IDLE)
    {
      StateManager::setState(States::PLAYING);
    }
    else if (StateManager::getState() == States::PLAYING)
    {
      StateManager::setState(States::IDLE);
    }
    // if (StateManager::getState() == HOMING_A || StateManager::getState() == HOMING_B)
    // {
    //   StateManager::setState(States::IDLE);
    // }
    // else
    // {
    //   StateManager::setState(States::HOMING_A);
    // }
  }

  if (btn_b.fell())
  {
    if (StateManager::getState() == States::IDLE)
    {
      dynamixel.rebootDynamixels();
      dynamixel.disableTorque();
      dynamixel.enableLEDs();

      StateManager::setState(States::HOMING_A);
    }
    else if (StateManager::getState() == HOMING_A || StateManager::getState() == HOMING_B)
    {
      currentFrame = 0;
      StateManager::setState(States::IDLE);
    }
  }

  // NetworkHandler::updateIncomingOSC();

  switch (StateManager::getState())
  {
  case States::IDLE:
    serialDataHandler.receiveAsCSV();
    if (ledBlinkTimer > ledBlinkIntervalIdleState)
    {
      LED_BUILTIN_STATE = !LED_BUILTIN_STATE;
      digitalWriteFast(LED_BUILTIN, LED_BUILTIN_STATE);
      ledBlinkTimer = 0;
    }
    break;

  case States::PLAYING:
    if (playTimer >= playFrameInterval)
    {
      playTimer = 0;
      SDCardHelpers::getFrame(currentFrame, frameData);
      // Serial.printf("#%d\t", currentFrame);
      // for (uint8_t i = 0; i < FRAME_DATA_SIZE / 2; i++)
      // {
      //   Serial.printf("%d\t", frameData[i]);
      // }
      // Serial.println();

      for (uint8_t i = 0; i < 4; i++)
      {
        serialDataHandler.serialData.targetPositionsSteppers[i] = long(frameData[i]);
      }
      // for (uint8_t i = 0; i < 11; i++)
      // {
      //   serialDataHandler.serialData.targetPositionsServos[i] = long(frameData[i + 3]);
      // }

      /*
      Serial.print("Target Positions Steppers: ");
      for (uint8_t i = 0; i < 3; i++)
      {
        Serial.print(serialDataHandler.serialData.targetPositionsSteppers[i]);
        Serial.print("\t");
      }
      Serial.println();
      Serial.print("Target Positions Dynamixels: ");
      for (uint8_t i = 0; i < 11; i++)
      {
        Serial.print(serialDataHandler.serialData.targetPositionsServos[i]);
        Serial.print("\t");
      }
      Serial.println();
      */

      stepperWrapper.setNewStepperPositions(serialDataHandler.serialData.targetPositionsSteppers);
      // dynamixel.setNewDynamixelPositions(serialDataHandler.serialData.targetPositionsServos);

      currentFrame++;
      if (currentFrame >= endFrame)
      {
        StateManager::setState(States::IDLE);
      }
    }
    stepperWrapper.runAllSpeedToPositions();
    break;

  case States::RUNNING:
    // serialDataHandler.receiveAsCSV();
    /* runSpeed(): Poll the motor and step it if a step is due, implementing a constant
                   speed as set by the most recent call to setSpeed().
                   You must call this as frequently as possible, but at least once per step interval.

       runSpeedToPosition(): Executes runSpeed() unless the targetPosition is reached. This function
                             needs to be called often just like runSpeed() or run().
                             Will step the motor if a step is required at the currently selected speed
                             unless the target position has been reached. Does not implement accelerations. */
    stepperWrapper.runAllSpeedToPositions();
    break;

  case States::MANUAL:
    // serialDataHandler.receiveAsCSV();
    /* run(): Poll the motor and step it if a step is due, implementing accelerations and decelerations to achieve the target position.
       You must call this as frequently as possible, but at least once per minimum step time interval, preferably in your main loop.
       Note that each call to run() will make at most one step, and then only when a step is due, based on the current speed and the time since the last step. */
    stepperWrapper.runAll();
    break;

  case States::HOMING_A:
    // if (ledBlinkTimer > ledBlinkIntervalIdleState / 16)
    // {
    //   LED_BUILTIN_STATE = !LED_BUILTIN_STATE;
    //   digitalWriteFast(LED_BUILTIN, LED_BUILTIN_STATE);
    //   ledBlinkTimer = 0;
    //   stepperWrapper.printEndSwitches();
    // }

    if (stepperWrapper.driveHoming_A())
    {
      stepperWrapper.zeroPositions();
      StateManager::setState(States::HOMING_B);
    }
    break;

  case States::HOMING_B:
    if (stepperWrapper.driveHoming_B())
    {
      // stepperWrapper.zeroPositions();
      dynamixel.disableLEDs();
      dynamixel.enableTorque();

      StateManager::setState(States::IDLE);
    }
    break;

  default:
    break;
  }

  // Enable LED_BUILTIN when a stepper is in running
  // TODO Enable again:
  // digitalWriteFast(LED_BUILTIN, stepper_0.isRunning() || stepper_1.isRunning() || stepper_2.isRunning());

  // handle_current_state();

  if (debugMessageTimer > debugMessageInterval)
  {
    debugMessageTimer = 0;
    Serial.print("State: ");
    Serial.println(StateManager::getStateAsString());
  }
}
