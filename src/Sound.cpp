#include "Sound.h"

#include <Arduino.h>
#include <ESP32Tone.h>

#include "Characters.h"
#include "Configuration.h"
#include "Utility.h"
#include "pitches.h"

Sound::Sound(Characters* characters) { this->characters = characters; }

void Sound::initialize() { pinMode(BUZZER_PIN, OUTPUT); }

void Sound::play(int frequency, int duration) {
  if (ENABLE_SOUND) {
    tone(BUZZER_PIN, frequency, duration);
  } else {
    delay(duration);
  }
}

void Sound::play(String character, int duration) {
  auto frequency = this->characters->getCharacterFrequency(character);
  if (frequency == 0) {
    // The character isn't on the wheel, so just ignore it.
    return;
  }
  this->play(frequency, duration);
}

void Sound::playLabel(String label) {
  // plays a music according to the label letters

  int length = Utility::utf8Length(label);

  for (int i = 0; i < length; i++) {
    int duration;
    // If the label is over 16 characters, decrease the note duration every
    // character starting at character 5.
    if (length > 16 && i > 4) {
      duration = max(NOTE_DURATION_MAX - (i - 4) * NOTE_DURATION_DECREASE,
                     NOTE_DURATION_MIN);
    } else {
      duration = NOTE_DURATION_MAX;
    }

    auto character = Utility::utf8CharAt(label, i);
    this->play(character, duration);
    delay(duration / 2);
  }
}

void Sound::playMelody(String notes, String durations) {
  int length = Utility::utf8Length(notes);

  for (int i = 0; i < length; i++) {
    auto character = Utility::utf8CharAt(notes, i);
    auto frequency = this->characters->getCharacterFrequency(character);
    if (frequency == 0) {
      continue;
    }
    char charDuration = durations[i];
    float duration = 2000 / atoi(&charDuration);
    this->play(frequency, duration);
  }
}
