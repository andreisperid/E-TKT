#pragma once

#include <Arduino.h>

#include "Configuration.h"

/**
 * @brief Logs messages to the serial port.  Very simple for now.
 */
class Logger {
 public:
  void initialize();
  void log(String message);
};
