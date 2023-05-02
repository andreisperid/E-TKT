#include "Characters.h"

#include <Arduino.h>
#include <U8g2lib.h>


void Characters::initialize() {
  // Iterate over the keys in CHARACTERS and return the maximum
  // value.  This is the number of characters in the wheel.
  this->characterCount = 0;
  for (auto const& x : CHARACTERS) {
    if (x.second > this->characterCount) {
      this->characterCount = x.second;
    }
  }

  this->characterCount++;
}

int Characters::getCharacterIndex(String character) {
  auto candidate = CHARACTERS.find(character);
  if (candidate == CHARACTERS.end()) {
    return -1;
  }
  return candidate->second;
}

FontInfo Characters::getFont(String character) {
  auto candidate = GLYPHS.find(character);
  if (candidate == GLYPHS.end()) {
    return FontInfo(u8g2_font_6x13_te, 0, 7, 0, 0);
  }
  return FontInfo(*(candidate->second));
}

int Characters::getCharacterFrequency(String character) {
  auto index = this->getCharacterIndex(character);
  if (index < 0) {
    return 0;
  }
  return NOTES[index];
}

int Characters::getWheelCharacterCount() { return this->characterCount; }