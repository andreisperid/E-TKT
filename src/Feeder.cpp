#include "Feeder.h"

#include <AccelStepper.h>
#include <Arduino.h>

#include "Configuration.h"
#include "Logger.h"


Feeder::Feeder(Logger* logger) {
  this->logger = logger;
  this->stepper = new AccelStepper(MICROSTEPS_FEED, 15, 2, 16, 4);
}

Feeder::~Feeder() { delete stepper; }

void Feeder::initialize() {
  this->stepper->setMaxSpeed(FEED_STEPPER_MAX_SPEED);
  this->stepper->setAcceleration(FEED_STEPPER_MAX_ACCELERATION);
}

void Feeder::feed(int repeat) {
  // runs the feed stepper by a specific amount to push the tape forward
  if (!ENABLE_FEED) {
    delay(500);
    return;
  }

  this->logger->log(String("Feeding ") + repeat + "x...");

  this->stepper->enableOutputs();
  delay(10);

  int direction = -1;
  if (REVERSE_FEED_STEPPER_DIRECTION) {
    direction = 1;
  }
  for (int i = 0; i < repeat; i++) {
    this->stepper->runToNewPosition(this->stepper->currentPosition() +
                                    (FEED_MOTOR_STEPS_PER_REVOLUTION / 8) *
                                        direction);
    delay(10);
  }

  this->deenergize();

  delay(20);
}

void Feeder::deenergize() { this->stepper->disableOutputs(); }
