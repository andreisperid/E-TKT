#pragma once

#include <Arduino.h>
#include <ESP32Servo.h>

#include "Configuration.h"
#include "Light.h"
#include "Logger.h"

#define REST_ANGLE 50
#define TARGET_ANGLE 22

/**
 * @brief Controlls a press connected to a servo.
 */
class Press {
 private:
  uint8_t pin;
  Logger* logger;
  Servo* servo;
  Light* pressLed;

 public:
  Press(Logger* logger, uint8_t pin, Light* pressLed);
  ~Press();

  /**
   * @brief Initializes the press.
   */
  void initialize();

  /**
   * @brief Presses the press.
   * Higher values for "force" will result in a stronger press.
   * strong=true will result in a stronger press that pushed furthur up.
   * slow=true will result in a slower press.
   */
  void press(bool strong, int force, bool slow);

  /**
   * @brief Moves the press to the rest position, away from the daisy wheel
   */
  void rest();
};
