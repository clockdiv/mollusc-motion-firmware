#include <Arduino.h>
#include <SD.h>
#include <SPI.h>
#include <WS2812Serial.h>

#include "averageFilter.h"
#include "pins.h"
#include "display.h"
#include "state.h"
#include "steppers.h"
#include "mm_neopixel.h"

#include "ReceiveSerialTest.h"
#include "DirectDriveTest.h"
#include "DynamixelTest.h"

void setup()
{
  Serial.begin(500000);

  delay(100);

  SDCard.initSD();
  SDCard.printAllFiles();

  delay(100);

  // log_sd("============================================================");
  // log_sd("starting board");
  // log_sd("============================================================");

  analogReadResolution(13);

  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(STEPPER_ENABLE, OUTPUT);
  digitalWriteFast(LED_BUILTIN, LOW);
  digitalWriteFast(STEPPER_ENABLE, LOW);

  stepper_1.setMinPulseWidth(minPulseWidth);
  stepper_1.setPinsInverted(false, false, false); // direction, step, enabled

  stepper_2.setMinPulseWidth(minPulseWidth);
  stepper_2.setPinsInverted(true, false, false); // direction, step, enabled

  stepper_3.setMinPulseWidth(minPulseWidth);
  stepper_3.setPinsInverted(false, false, false); // direction, step, enabled

  digitalWriteFast(STEPPER_ENABLE, LOW); // enable all steppers

  btn_a.attach(BTN_A, INPUT_PULLUP);
  btn_a.interval(25);
  btn_a.setPressedState(LOW);

  btn_b.attach(BTN_B, INPUT_PULLUP);
  btn_b.interval(25);
  btn_b.setPressedState(LOW);

  end_1.attach(STEPPER_1_END, INPUT_PULLUP);
  end_1.interval(25);
  end_1.setPressedState(LOW);

  end_2.attach(STEPPER_2_END, INPUT_PULLUP);
  end_2.interval(25);
  end_2.setPressedState(LOW);

  end_3.attach(STEPPER_3_END, INPUT_PULLUP);
  end_3.interval(25);
  end_3.setPressedState(LOW);

  init_dxl();
  init_neopixels();

  for (int i = 0; i < 1024; i++)
  {
    poti_a = poti_a_filtered.filter(analogRead(POTI_A));
    poti_b = poti_b_filtered.filter(analogRead(POTI_B));
  }
  poti_a_old = poti_a;
  poti_b_old = poti_b;

  set_state(IDLE);

  // log_sd("setup done, starting mainloop");

  previous_millis = millis();
}

void loop()
{
  current_millis = millis();

  btn_a.update();
  btn_b.update();
  end_1.update();
  end_2.update();
  end_3.update();

  // fade neopixels
  float neopixel_brightness = (sin(millis() / 500.0) + 1.0) / 2.0 * 55.0;
  set_all_neopixels(WHITE, int(neopixel_brightness));

  // End-Switch / Button Test
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
    if (state == HOMING_A || state == HOMING_B)
    {
      set_state(IDLE);
    }
    else
    {
      set_state(HOMING_A);
    }
  }

  if (btn_b.fell())
  {
    Serial.println("Rebooting Dynamixels");
    disableTorque();
    enableLEDs();
    delay(2000);
    rebootDynamixels();
    disableLEDs();
    enableTorque();

    delay(300);
    enableLEDs();
    delay(300);
    disableLEDs();
  }

  switch (state)
  {
  case IDLE:
    ReceiveSerial();
    break;
  case RUNNING:
    ReceiveSerial();
    /* runSpeed(): Poll the motor and step it if a step is due, implementing a constant
                   speed as set by the most recent call to setSpeed().
                   You must call this as frequently as possible, but at least once per step interval.

       runSpeedToPosition(): Executes runSpeed() unless the targetPosition is reached. This function
                             needs to be called often just like runSpeed() or run().
                             Will step the motor if a step is required at the currently selected speed
                             unless the target position has been reached. Does not implement accelerations. */
    stepper_1.runSpeedToPosition();
    stepper_2.runSpeedToPosition();
    stepper_3.runSpeedToPosition();

    break;
  case MANUAL:
    ReceiveSerial();
    /* run(): Poll the motor and step it if a step is due, implementing accelerations and decelerations to achieve the target position.
       You must call this as frequently as possible, but at least once per minimum step time interval, preferably in your main loop.
       Note that each call to run() will make at most one step, and then only when a step is due, based on the current speed and the time since the last step. */

    stepper_1.run();
    stepper_2.run();
    stepper_3.run();
    break;
  case HOMING_A:
    if (driveHoming_A())
    {
      set_state(HOMING_B);
    }
    break;
  case HOMING_B:
    if (driveHoming_B())
    {
      zeroPositions();
      set_state(IDLE);
    }
    break;
  default:
    break;
  }

  // Enable LED_BUILTIN when a stepper is running
  digitalWriteFast(LED_BUILTIN, stepper_1.isRunning() || stepper_2.isRunning() || stepper_3.isRunning());

  // handle_current_state();
}
