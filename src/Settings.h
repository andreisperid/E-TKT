#pragma once

#include <Arduino.h>
#include <preferences.h>

#include <tuple>

#include "Configuration.h"
#include "Logger.h"

#define DEFAULT_ALIGN_FACTOR 5  // 1 to 9, 5 is the mid value
#define DEFAULT_FORCE_FACTOR 1  // 1 to 9

/**
 * @brief A class for storing and retrieving settings from EEPROM using the
 * preference library.
 */
class Settings {
 private:
  Logger* logger;
  Preferences* preferences = new Preferences();
  uint alignFactor = 0;
  uint forceFactor = 0;

 public:
  Settings(Logger* logger);
  ~Settings();

  /**
   * @brief Initializes the settings object by loading the settings from EEPROM.
   */
  void initialize();

  /**
   * @brief Returns the current alginment calbration value
   */
  uint32_t getAlignFactor();

  /**
   * @brief Returns the current force calibration value
   */
  uint32_t getForceFactor();

  /**
   * @brief Saves the given alignment and force calibration values to EEPROM.
   */
  void save(uint32_t newAlignFactor, uint32_t newForceFactor);
};
