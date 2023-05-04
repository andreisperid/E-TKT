#include "Press.h"

#include <Arduino.h>
#include <ESP32Servo.h>

#include "Configuration.h"
#include "Light.h"
#include "Logger.h"


Press::Press(Logger* logger, uint8_t pin, Light* pressLed) {
  this->logger = logger;
  this->pin = pin;
  this->pressLed = pressLed;
  this->servo = new Servo();
}

void Press::initialize() {
  // set  servo
  this->servo->attach(this->pin);
  this->rest();
  delay(100);
}

void Press::press(bool strong, int force, bool slow) {
  if (!ENABLE_PRESS) {
    delay(500);
    return;
  }

  this->logger->log("Pressing...");

  int delayFactor = 0;

  auto peakAngle = TARGET_ANGLE + 9 - ASSEMBLY_CALIBRATION_FORCE - force;

  if (strong) {
    delayFactor = 4;
  } else {
    delayFactor = slow ? 100 : 0;
  }
  this->pressLed->on(1.0f);  // lights up the char led

  for (int pos = REST_ANGLE; pos >= peakAngle; pos--) {
    this->servo->write(pos);
    delay(delayFactor);
  }
  for (int i = 0; i < 5;
       i++)  // to make sure the servo has reached the peak position
  {
    this->servo->write(peakAngle);
    delay(50);
  }

  for (int pos = peakAngle; pos <= REST_ANGLE; pos++) {
    this->servo->write(pos);
    delay(delayFactor);
  }
  for (int i = 0; i < 5;
       i++)  // to make sure the servo has reached the rest position
  {
    this->servo->write(REST_ANGLE);
    delay(50);
  }

  this->pressLed->on(0.2f);  // dims the char led
}

void Press::rest() { this->servo->write(REST_ANGLE); }
