#include "ETKT.h"

#include <Arduino.h>
#include <FreeRTOS.h>

#include <map>
#include <mutex>
#include <thread>

#include "DaisyWheel.h"
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


ETKT::ETKT(Logger* logger, Settings* settings, Characters* characters,
           Display* display, DaisyWheel* daisywheel, HallSwitch* hall,
           Feeder* feeder, Press* press, Sound* sound, Light* ledFinish,
           Light* ledChar) {
  this->settings = settings;
  this->display = display;
  this->daisywheel = daisywheel;
  this->hall = hall;
  this->feeder = feeder;
  this->press = press;
  this->sound = sound;
  this->ledFinish = ledFinish;
  this->ledChar = ledChar;
  this->characters = characters;

  this->command = NULL;
  this->progress = 0;
  this->lock = new std::mutex();
  this->eventGroup = xEventGroupCreate();
}

ETKT::~ETKT() {
  // Empty for now
}

void ETKT::initialize() {
  this->logger->initialize();
  this->characters->initialize();
  this->ledFinish->initialize();
  this->ledChar->initialize();
  this->sound->initialize();
  this->settings->initialize();
  this->display->initialize();
  this->hall->initialize();
  this->press->initialize();
  this->feeder->initialize();
  this->daisywheel->initialize();
}

StatusUpdate* ETKT::createStatus() {
  auto status = new StatusUpdate();
  this->lock->lock();
  if (this->command != NULL) {
    status->currentCommand = this->command->command;
    status->currentCommandString = this->command->commandAsString();
    status->currentLabel = this->command->label;
    status->progress = this->progress;
  }
  this->lock->unlock();
  status->align = this->settings->getAlignFactor();
  status->force = this->settings->getForceFactor();
  return status;
}

void ETKT::cutCommand() {
  auto command = new CommandOptions();
  command->command = Command::CUT;
  this->startCommand(command);
}

void ETKT::feedCommand() {
  auto command = new CommandOptions();
  command->command = Command::FEED;
  this->startCommand(command);
}

void ETKT::reelCommand() {
  auto command = new CommandOptions();
  command->command = Command::REEL;
  this->startCommand(command);
}

void ETKT::testAlignCommand(int align) {
  auto command = new CommandOptions();
  command->command = Command::TEST_ALIGN;
  command->align = align;
  this->startCommand(command);
}

void ETKT::testFullCommand(int align, int force) {
  auto command = new CommandOptions();
  command->command = Command::TEST_FULL;
  command->align = align;
  command->force = force;
  this->startCommand(command);
}

void ETKT::saveCommand(int align, int force) {
  auto command = new CommandOptions();
  command->command = Command::SAVE;
  command->align = align;
  command->force = force;
  this->startCommand(command);
}

void ETKT::homeCommand() {
  auto command = new CommandOptions();
  command->command = Command::HOME;
  this->startCommand(command);
}

void ETKT::moveCommand(String character) {
  auto command = new CommandOptions();
  command->command = Command::MOVE;
  command->label = character;
  this->startCommand(command);
}

void ETKT::tagCommand(String label) {
  auto command = new CommandOptions();
  command->command = Command::TAG;
  command->label = label;
  this->startCommand(command);
}

void ETKT::cut() {
  if (!ENABLE_CUT) {
    delay(500);
    return;
  }
  // moves to a specific char (*) then presses label three times (more
  // vigorously)
  this->daisywheel->move("*", this->settings->getAlignFactor());
  for (int i = 0; i < 3; i++) {
    this->press->press(true, this->settings->getForceFactor(), false);
  }
}

void ETKT::startCommand(CommandOptions* command) {
  this->lock->lock();
  if (this->command != NULL) {
    this->lock->unlock();
    delete command;
    throw "Command already in progress";
  }
  this->command = command;
  xEventGroupSetBits(this->eventGroup, BIT0);
  this->lock->unlock();
}

void ETKT::loop() {
  // Wait for the signal, with a timeout of 500 ms just in case.
  xEventGroupWaitBits(eventGroup, BIT0, pdTRUE, pdFALSE,
                      500 / portTICK_PERIOD_MS);

  // check for a command
  this->lock->lock();
  if (this->command == NULL) {
    this->lock->unlock();
    return;
  }
  this->lock->unlock();

  // Do the task
  switch (this->command->command) {
    case Command::CUT:
      this->cutCommandInternal();
      break;
    case Command::FEED:
      this->feedCommandInternal();
      break;
    case Command::REEL:
      this->reelCommandInternal();
      break;
    case Command::TEST_ALIGN:
      this->testCommandInternal();
      break;
    case Command::TEST_FULL:
      this->testCommandFullInternal();
      break;
    case Command::SAVE:
      this->saveCommandInternal();
      break;
    case Command::TAG:
      this->tagCommandInternal();
      break;
    case Command::HOME:
      this->homeCommandInternal();
      break;
    case Command::MOVE:
      this->moveCommandInternal();
      break;
    case Command::IDLE:
      break;
  }

  this->lock->lock();

  // Clean up the command
  delete this->command;
  this->command = NULL;
  this->progress = 0;

  // Turn everything off
  this->display->renderIdle();
  this->daisywheel->deenergize();
  this->press->rest();
  this->feeder->deenergize();
  this->lock->unlock();
}

void ETKT::feedCommandInternal() {
  this->display->renderFeed();
  this->ledFinish->on(1.0f / 8);
  this->press->rest();
  delay(500);
  this->feeder->feed();
  this->ledFinish->off();
  this->ledChar->off();
}

void ETKT::reelCommandInternal() {
  this->display->renderReel();
  this->ledFinish->on(1.0f / 8);
  this->press->rest();
  delay(500);

  this->feeder->feed(16);
  this->ledFinish->off();
  this->ledChar->off();
}

void ETKT::cutCommandInternal() {
  this->display->renderCut();
  this->ledChar->on(0.2f);
  this->press->rest();
  delay(500);

  this->cut();
  ledChar->off();
}

void ETKT::saveCommandInternal() {
  this->logger->log("saving settings");

  display->initialize();
  display->renderSettings(this->command->align, this->command->force);
  ledFinish->off();

  settings->save(this->command->align, this->command->force);
  display->renderReboot();
  ledFinish->off();
  ledChar->off();
  delay(500);

  // TODO: Does the device actually need to reboot?
  // I think all the state gets updated properly without it.
  ESP.restart();
}

void ETKT::testCommandInternal() {
  display->renderTest(this->command->align, this->command->force);
  ledFinish->off();

  this->daisywheel->move("M", this->command->align);
  this->press->press(false, 1, true);
}

void ETKT::testCommandFullInternal() {
  String label = "E-TKT";
  this->feeder->feed();
  for (int i = 0; i < label.length(); i++) {
    auto character = label.substring(i, i + 1);
    this->feeder->feed();
    this->daisywheel->move(character, this->command->align);
    this->press->press(false, this->command->force, false);
  }
  this->feeder->feed();
  this->daisywheel->move("*", this->command->align);
  this->cut();
}

void ETKT::homeCommandInternal() {
  this->ledFinish->on(1.0f);
  this->ledChar->on(1.0f);
  this->press->rest();
  delay(500);
  this->daisywheel->home(this->settings->getAlignFactor());
  delay(1000);
}

void ETKT::moveCommandInternal() {
  this->press->rest();
  delay(500);
  this->daisywheel->move(this->command->label, this->settings->getAlignFactor());
}

void ETKT::tagCommandInternal() {
  auto label = this->command->label;
  label.toUpperCase();
  // enables servo
  this->press->rest();
  delay(500);

  this->logger->log(String("print ") + label);

  // all possible characters: $-.23456789*abcdefghijklmnopqrstuvwxyz♡☆♪€@
  int labelLength = Utility::utf8Length(label);

  this->ledChar->on(0.2f);

  this->display->renderProgress(0, label);

  if (label == " TASCHENRECHNER " || label == " POCKET CALCULATOR " ||
      label == " DENTAKU " || label == " CALCULADORA " ||
      label == " MINI CALCULATEUR ") {
    this->sound->playMelody(
        "*4599845887*459984588764599845887*4599845887",
        "88843888484888438884848884388848488843888484");  // ♪ I'm the operator
                                                          // with my pocket
                                                          // calculator ♪
  } else {
    this->sound->playLabel(label);
  }

  // home daisy wheel
  this->daisywheel->home(this->settings->getAlignFactor());

  this->feeder->feed();

  for (int i = 0; i < labelLength; i++) {
    auto character = Utility::utf8CharAt(label, i);
    if (character != " ") {
      this->daisywheel->move(character, this->settings->getAlignFactor());
      this->press->press(false, this->settings->getForceFactor(), false);
    }

    this->feeder->feed();
    delay(500);

    this->display->renderProgress(i + 1, label);

    this->lock->lock();
    this->progress = 100.0f * (i + 1) / labelLength;
    if (this->progress >= 100) {
      this->progress = 99;  // avoid 100% progress while still finishing
    }
    this->lock->unlock();
  }

  if (labelLength < 6 &&
      labelLength !=
          1)  // minimum label length to make sure the user can grab it
  {
    int spaceDelta = 6 - labelLength;
    for (int i = 0; i < spaceDelta; i++) {
      this->feeder->feed();
    }
  }

  this->cut();

  this->ledChar->off();
  display->renderFinished();

  this->logger->log("Blinking LED");
  // Blink the finish led a few times.
  for (int i = 0; i < 5; i++) {
    this->ledFinish->off();
    delay(100);
    this->ledFinish->on(0.5f);
    delay(100);
  }

  this->logger->log("Fading LED");
  // Then fade it out.
  for (int i = 128; i >= 0; i--) {
    this->ledFinish->on(i / 128.0f);
    delay(25);
  }
  this->ledFinish->off();
  this->logger->log("Printing Complete");
}
