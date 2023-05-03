// MIT License

// Copyright (c) 2022 Andrei Speridião

// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:

// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

//
// for more information, please visit https://github.com/andreisperid/E-TKT
//

// vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
//
// IMPORTANT: do not forget to upload the files in "data" folder using SPIFFS
//
// ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

#include <Arduino.h>

#include "DaisyWheel.h"
#include "Characters.h"
#include "Configuration.h"
#include "Display.h"
#include "ETKT.h"
#include "Feeder.h"
#include "HallSwitch.h"
#include "Light.h"
#include "Logger.h"
#include "Network.h"
#include "Press.h"
#include "Settings.h"
#include "Sound.h"
#include "Utility.h"

Logger* logger = new Logger();
Characters* characters = new Characters();
Sound* sound = new Sound(characters);
Display* display = new Display(sound, characters);
Settings* settings = new Settings(logger);
Light* ledFinish = new Light(FINISH_LED_PIN);
Light* ledChar = new Light(CHARACTER_LED_PIN);
Press* press = new Press(logger, SERVO_PIN, ledChar);
HallSwitch* hall = new HallSwitch(logger, HALL_PIN);
DaisyWheel* daisywheel = new DaisyWheel(logger, hall, characters, settings);
Feeder* feeder = new Feeder(logger);
ETKT* etkt = new ETKT(logger, settings, characters, display, daisywheel, hall,
                      feeder, press, sound, ledFinish, ledChar);
Network* network = new Network(logger, display, etkt, WIFI_RESET_PIN);

void setup() {
  // Initialize hardware
  etkt->initialize();

  // Play the splash screen
  display->playSplashScreen();

  // Start WiFi, network.
  network->initialize();

  // Display the ready "idle" screen.
  display->renderIdle();
}

void loop() { etkt->loop(); }
