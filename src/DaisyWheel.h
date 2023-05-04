#pragma once

#include <AccelStepper.h>

#include "Arduino.h"
#include "Characters.h"
#include "Configuration.h"
#include "HallSwitch.h"
#include "Logger.h"
#include "Settings.h"

/**
 * @brief Controls the daisy wheel stepper motor and computes the geometry
 * necessary for moving it into the correct position.
 */
class DaisyWheel {
 private:
  Logger* logger;
  HallSwitch* hall;
  Settings* settings;
  AccelStepper* stepper;
  Characters* characters;
  const int stepsPerRevolution = CHAR_STEP_COUNT * CHAR_MICROSTEPS;
  float stepsPerChar = 0;
  int currentChar;

 public:
  DaisyWheel(Logger* logger, HallSwitch* hall, Characters* characters,
           Settings* settings);
  ~DaisyWheel();

  /**
   * @brief Initializes the daisy wheel stepper motor and homes it.
   */
  void initialize();

  /**
   * @brief Moves the daisy wheel to the home position, which shoudl be letter "J".
   */
  void home(int align);

  /**
   * @brief Moves the daisy wheel to the provided character "c".
   */
  bool move(String c, int alignFactor);

  /**
   * @brief Deactivates the daisy wheel stepper motor, potentially losing its
   * position.
   */
  void deenergize();
};
