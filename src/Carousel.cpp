#include "Carousel.h"

#include <AccelStepper.h>

#include "Arduino.h"
#include "Characters.h"
#include "Configuration.h"
#include "HallSwitch.h"
#include "Logger.h"
#include "Settings.h"

Carousel::Carousel(Logger* logger, HallSwitch* hall, Characters* characters,
                   Settings* settings) {
  this->logger = logger;
  this->hall = hall;
  this->characters = characters;
  this->settings = settings;
  this->stepper = new AccelStepper(AccelStepper::DRIVER, PIN_STEPPER_CHAR_STEP,
                                   PIN_STEPPER_CHAR_DIR);
}

Carousel::~Carousel() { delete stepper; }

void Carousel::initialize() {
  this->stepsPerChar = (float)this->stepsPerRevolution /
                       this->characters->getWheelCharacterCount();
  digitalWrite(PIN_STEPPER_CHAR_ENABLE, HIGH);
  this->stepper->setMaxSpeed(CHARACTER_STEPPER_MAX_SPEED);
  this->stepper->setAcceleration(CHARACTER_STEPPER_MAX_ACCELERATION);
  this->stepper->setPinsInverted(true, false, true);
  this->stepper->setEnablePin(PIN_STEPPER_CHAR_ENABLE);
  this->home(this->settings->getAlignFactor());  // initial home for reference
  this->deenergize();
}

void Carousel::home(int align) {
  this->stepper->enableOutputs();
  // runs the char stepper clockwise until triggering the hall sensor, then call
  // it home at char 21
  auto a = (align - 5.0f) / 10.0f;

  logger->log(String("Homing with align: ") + align + " and a: " + a);

  // Check to see if the hall sensor on the stepper is already trigerred
  // and if so, move it a little bit to get the sensor into an un-trigerred
  // position.
  if (this->hall->triggered()) {
    long position = -this->stepsPerChar * 4;
    logger->log(String("Moving to position: ") + position +
                " because the hall sensor is already triggered.");
    this->stepper->runToNewPosition(-this->stepsPerChar * 4);
    this->stepper->run();
  }

  // TODO: Change the above to only move as long as the hall sensor is
  // triggerred, which could save a little time while printing.

  // Move the carousel until the hall sensor triggers, and then treat wherever
  // that is as the new home position.
  this->stepper->move(-this->stepsPerRevolution * 1.5f);
  auto hallState = hall->triggered();
  while (!hallState) {
    this->stepper->run();
    // TODO: less intrusive way to avoid triggering watchdog?
    delayMicroseconds(100);

    hallState = hall->triggered();
  }
  // TODO: Add a failure path for if the stepper moved a full rotation without
  // trigerring the sensor, inidcating that something is wrong with the
  // hardware.
  this->stepper->setCurrentPosition(0);

  this->stepper->runToNewPosition(-stepsPerChar + (stepsPerChar * a) +
                                  (ASSEMBLY_CALIBRATION_ALIGN * stepsPerChar));
  this->stepper->run();
  this->stepper->setCurrentPosition(0);
  this->currentChar = CHAR_HOME_POSITION;

  delay(100);
}

bool Carousel::move(String c, int alignFactor) {
  if (!ENABLE_CAROUSEL) {
    delay(500);
    return true;
  }

  // reaches out for a specific character
  logger->log(String("Moving to character ") + c);
  this->stepper->enableOutputs();
  auto charIndex = this->characters->getCharacterIndex(c);
  if (charIndex < 0) {
    // Invalid character
    return false;
  }
  if (charIndex == this->currentChar) {
    // No need to move, we're already there.
    logger->log("Already in position");
    return true;
  }

  // calls home everytime to avoid accumulating errors
  this->home(alignFactor);

  auto charDelta = charIndex - this->currentChar;
  logger->log(String("New character index is ") + charIndex +
              " and character delta is " + charDelta);
  // matches the character to the list and gets delta steps from home
  while (charDelta < 0) {
    charDelta += this->characters->getWheelCharacterCount();
  }

  if (charDelta == 0) {
    // No need to move, we're already there
    logger->log("Already in position");
    return true;
  }

  // runs char stepper clockwise to reach the target position
  long position = -this->stepsPerChar * charDelta;
  logger->log(String("Moving ") + charDelta + " characters to position " +
              position);
  this->stepper->runToNewPosition(position);
  this->currentChar = charIndex;

  delay(25);
  return true;
}

void Carousel::deenergize() {
  this->stepper->disableOutputs();

  // Disabling the motors loses position information, so we need to reset it.
  this->currentChar = -1;
}