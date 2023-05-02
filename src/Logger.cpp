#include "Logger.h"

#include <Arduino.h>

#include "Configuration.h"


void Logger::initialize() {
  if (ENABLE_SERIAL) {
    Serial.begin(115200);

    // Log a nifty logo.
    Serial.println("");
    Serial.println(" _____    _____  _  __ _____  ");
    Serial.println("/  __/   /__ __\\/ |/ //__ __\\ ");
    Serial.println("|  \\ _____ / \\  |   /   / \\   ");
    Serial.println("|  /_\\____\\| |  |   \\   | |   ");
    Serial.println("\\____\\     \\_/  \\_|\\_\\  \\_/   ");
    Serial.println("");
    Serial.println("Anachronistic label maker designed by");
    Serial.println("https://andrei.cc and made by you!");
    Serial.println("");
  }
}

void Logger::log(String message) {
  if (ENABLE_SERIAL) {
    Serial.println(message);
  }
}