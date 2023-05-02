#pragma once

#include <Arduino.h>
#include <ESP32Tone.h>

#include "Characters.h"
#include "Configuration.h"
#include "Utility.h"
#include "pitches.h"  // list of notes and their frequencies


#define NOTE_DURATION_MAX 100
#define NOTE_DURATION_MIN 20
#define NOTE_DURATION_DECREASE 2

/**
 * @brief A class for controlling the buzzer to play single notes, melodies, and
 * songs based on a label.
 */
class Sound {
 private:
  Characters* characters;

 public:
  Sound(Characters* characters);
  ~Sound();

  /**
   * @brief Starts and sets up the buzzer.
   */
  void initialize();

  /**
   * @brief Plays a note on the buzzer.
   */
  void play(int frequency = 2000, int duration = 1000);

  /**
   * @brief Plays a note on the buzzer based on its character.
   */
  void play(String character, int duration = 1000);

  /**
   * @brief Plays a tune on the buzzer based on the label text.
   */
  void playLabel(String label);

  /**
   * @brief Plays a melody on the buzzer.
   * @details ♪ By pressing down a special key ♪
   *          ♪ It plays a little melody ♪
   */
  void playMelody(String notes, String durations);
};
