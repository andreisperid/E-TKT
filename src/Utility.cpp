#include "Utility.h"

#include <Arduino.h>


int Utility::utf8CharLength(String str, int position) {
  int start = str[position];
  if (start >> 3 == 30) {
    return 4;
  } else if (start >> 4 == 14) {
    return 3;
  } else if (start >> 5 == 6) {
    return 2;
  } else {
    return 1;
  }
}

// Returns the length of a UTF-8 encoded string, treating each UTF-8 code-point
// as a single character.
int Utility::utf8Length(String str) {
  int position = 0;
  int length = 0;
  while (position < str.length()) {
    position += Utility::utf8CharLength(str, position);
    length++;
  }
  return length;
}

// Returns the UTF-8 characters at the given position of the given string,
// treating multi-character code points as inidividual characters.
String Utility::utf8CharAt(String str, int position) {
  int current = 0;
  while (current < str.length() && position > 0) {
    current += Utility::utf8CharLength(str, current);
    position--;
  }
  int charLength = Utility::utf8CharLength(str, current);
  return str.substring(current, current + charLength);
}
