#include "Settings.h"

#include <Arduino.h>
#include <preferences.h>

#include <tuple>

#include "Configuration.h"
#include "Logger.h"


Settings::Settings(Logger* logger) { this->logger = logger; }

void Settings::initialize() {
  // load settings from internal memory
  this->preferences->begin("calibration", false);

  // check if are there any align and force values stored in memory
  int a = this->preferences->getUInt("align", 0);
  if (a == 0) {
    this->preferences->putUInt("align", DEFAULT_ALIGN_FACTOR);
  }
  int f = this->preferences->getUInt("force", 0);
  if (f == 0) {
    this->preferences->putUInt("force", DEFAULT_FORCE_FACTOR);
  }

  this->alignFactor = this->preferences->getUInt("align", 0);
  this->forceFactor = this->preferences->getUInt("force", 0);
  this->preferences->end();

  this->logger->log(String("Align factor: ") + this->alignFactor);
  this->logger->log(String("Force factor: ") + this->forceFactor);
}

void Settings::save(uint32_t newAlignFactor, uint32_t newForceFactor) {
  // save settings to memory

  this->preferences->begin("calibration", false);

  this->preferences->putUInt("align", newAlignFactor);
  this->preferences->putUInt("force", newForceFactor);

  this->alignFactor = newAlignFactor;
  this->forceFactor = newForceFactor;

  this->logger->log(String("Saved align ") + alignFactor);
  this->logger->log(String("Saved force ") + forceFactor);

  this->preferences->end();
}

uint32_t Settings::getAlignFactor() { return this->alignFactor; }

uint32_t Settings::getForceFactor() { return this->forceFactor; }
