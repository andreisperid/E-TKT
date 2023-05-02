#pragma once

#include <AccelStepper.h>
#include <Arduino.h>

#include "Configuration.h"
#include "Logger.h"

/**
 * @brief Controls the feeder stepper motor.
 *
 * This motor feeds the label tape in only one direction and never needs to
 * maintain its position.
 */
class Feeder {
 private:
  Logger* logger;
  AccelStepper* stepper;

 public:
  Feeder(Logger* logger);
  ~Feeder();
  void initialize();
  void feed(int repeat = 1);
  void deenergize();
};
