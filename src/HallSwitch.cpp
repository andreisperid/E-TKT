#include "HallSwitch.h"

#include <Arduino.h>

#include "Configuration.h"
#include "Logger.h"


HallSwitch::HallSwitch(Logger* logger, uint8_t pin) {
  this->logger = logger;
  this->pin = pin;
}

void HallSwitch::initialize() { pinMode(this->pin, INPUT_PULLUP); }

bool HallSwitch::triggered() {
  return (analogRead(this->pin) < HALL_SENSOR_THRESHOLD) ^
         INVERT_HALL_SENSOR_LOGIC;
}
