#pragma once

#include <AccelStepper.h>

#include "Arduino.h"
#include "Characters.h"
#include "Configuration.h"
#include "HallSwitch.h"
#include "Logger.h"
#include "Settings.h"

/**
 * @brief Controls the carousel stepper motor and computes the geometry
 * necessary for moving it into the correct position.
 */
class Carousel {
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
  Carousel(Logger* logger, HallSwitch* hall, Characters* characters,
           Settings* settings);
  ~Carousel();

  /**
   * @brief Initializes the carousel stepper motor and homes it.
   */
  void initialize();

  /**
   * @brief Moves the carousel to the home position, which shoudl be letter "J".
   */
  void home(int align);

  /**
   * @brief Moves the carousel to the provided character "c".
   */
  bool move(String c, int alignFactor);

  /**
   * @brief Deactivates the carousel stepper motor, potentially losing its
   * position.
   */
  void deenergize();
};
