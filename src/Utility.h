#pragma once

#include <Arduino.h>

class Utility {
 public:
  /**
   * @brief Returns the length of a UTF-8 encoded string, treating each UTF-8
   * code-point as a single character.
   */
  static int utf8Length(String str);

  /**
   * @brief Returns the UTF-8 characters at the given position of the given
   * string, treating multi-character code points as inidividual characters.
   */
  static String utf8CharAt(String str, int position);

  /**
   * @brief Returns the number of bytes making up the UTF-8 character starting
   * at char index
   * @see https://en.wikipedia.org/wiki/UTF-8#Encoding
   */
  static int utf8CharLength(String str, int position);
};
