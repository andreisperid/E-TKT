#pragma once

#include <Arduino.h>
#include <FreeRTOS.h>

#include <map>
#include <mutex>
#include <thread>

#include "Carousel.h"
#include "Characters.h"
#include "Configuration.h"
#include "Display.h"
#include "Feeder.h"
#include "HallSwitch.h"
#include "Light.h"
#include "Logger.h"
#include "Press.h"
#include "Settings.h"
#include "Sound.h"

/**
 * @brief The different types of command the E-TKT can execute.
 */
enum Command {
  CUT = 0,
  FEED = 1,
  REEL = 2,
  TEST_ALIGN = 3,
  TEST_FULL = 4,
  SAVE = 5,
  TAG = 6,
  HOME = 7,
  MOVE = 8,
  IDLE = 9
};

/**
 * @brief A struct for storing the options for a command.
 *
 * The struct includes the superset of options a command can include. The
 * command itself determines which options are relevant.
 */
struct CommandOptions {
  Command command = Command::IDLE;
  String label = "";
  int align = 0;
  int force = 0;

  ~CommandOptions() { this->label = ""; }

  String commandAsString() {
    switch (this->command) {
      case Command::CUT:
        return "cut";
      case Command::FEED:
        return "feed";
      case Command::REEL:
        return "reel";
      case Command::TEST_ALIGN:
        return "testalign";
      case Command::TEST_FULL:
        return "testfull";
      case Command::SAVE:
        return "save";
      case Command::TAG:
        return "tag";
      case Command::HOME:
        return "home";
      case Command::MOVE:
        return "move";
      case Command::IDLE:
        return "idle";
      default:
        return "unknown";
    }
  }
};

/**
 * @brief A struct for storing the current status of the E-TKT.
 *
 * This is primarily used to communicate that device status to the webapp.
 */
struct StatusUpdate {
  float progress = 0;
  bool busy = false;
  int align = 0;
  int force = 0;
  String currentLabel = "";
  String currentCommandString = "";
  Command currentCommand = Command::IDLE;

  ~StatusUpdate() {
    this->currentLabel = "";
    this->currentCommandString = "";
  }
};

class ETKT {
 private:
  // Device hardware
  Logger* logger;
  Light* ledFinish;
  Light* ledChar;
  Settings* settings;
  Display* display;
  Carousel* carousel;
  HallSwitch* hall;
  Feeder* feeder;
  Press* press;
  Sound* sound;
  Characters* characters;

  // Device state, which should onyl ever be modified inside an exclusive lock.
  CommandOptions* command = NULL;
  float progress;
  std::mutex* lock;

  // Event group that the main loop blocks on for new commands.
  EventGroupHandle_t eventGroup;

  void cut();

  /**
   * Interanl handlers for each type of command the device can do.
   */
  void cutCommandInternal();
  void feedCommandInternal();
  void reelCommandInternal();
  void testCommandInternal();
  void testCommandFullInternal();
  void saveCommandInternal();
  void homeCommandInternal();
  void moveCommandInternal();
  void tagCommandInternal();

  /**
   * @brief Starts the given command or rejects it if the device is busy
   */
  void startCommand(CommandOptions* command);

 public:
  ETKT(Logger* logger, Settings* settings, Characters* characters,
       Display* display, Carousel* carousel, HallSwitch* hall, Feeder* feeder,
       Press* press, Sound* sound, Light* ledFinish, Light* ledChar);
  ~ETKT();

  /**
   * @brief Initializes the E-TKT by initializing each component of hardware.
   */
  void initialize();

  /**
   * The main loop for the device, which proceses commands as they come in.
   */
  void loop();

  /**
   * Public handlers for each command, expected to be called by the Webserver.
   *
   * Each handler is responsible for creating and populating a CommandOptions
   * struct and calling startCommand() with it.
   */
  void cutCommand();
  void feedCommand();
  void reelCommand();
  void testAlignCommand(int align);
  void testFullCommand(int align, int force);
  void saveCommand(int align, int force);
  void homeCommand();
  void moveCommand(String character);
  void tagCommand(String label);

  /**
   * @brief Returns the current status of the device, ie the printing status.
  */
  StatusUpdate* createStatus();
};
