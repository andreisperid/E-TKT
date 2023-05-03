#pragma once

#include <Arduino.h>

#include "Configuration.h"


/**
 * @brief Controls an LED connected to a PWM pin.
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
