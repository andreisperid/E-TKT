#pragma once

#include <Arduino.h>

#include "Configuration.h"
#include "Logger.h"

/**
 * @brief Controls a hall effect sensor.
 * 
 * The hall effect sensor is used to detect when the feeder has rotated to the
 * home position.
 */
class HallSwitch {
 private:
  Logger* logger;
  uint8_t pin;

 public:
  HallSwitch(Logger* logger, uint8_t pin);
  ~HallSwitch();
  void initialize();
  bool triggered();
};
