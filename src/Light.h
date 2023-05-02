#pragma once

#include <Arduino.h>

#include "Configuration.h"


/**
 * @brief Controls a light connected to an analog pin.
 */
class Light {
 private:
  uint8_t pin;

 public:
  Light(uint8_t pin);
  ~Light();
  void initialize();
  void on(float brightness);
  void off();
};
