#include "Display.h"

#include <Arduino.h>
#include <U8g2lib.h>
#include <qrcode.h>

#include "Characters.h"
#include "Configuration.h"
#include "Sound.h"
#include "Utility.h"
#include "etktLogo.h"


Display::Display(Sound *sound, Characters *characters) {
  this->u8g2 = new U8G2_SSD1306_128X64_NONAME_F_HW_I2C(U8G2_R0, U8X8_PIN_NONE);
  this->sound = sound;
  this->characters = characters;
}

Display::~Display() {
  delete this->u8g2;
  delete this->qrcode;
}

void Display::initialize() {
  // starts and sets up the display
  this->u8g2->begin();
  this->u8g2->clearBuffer();
  this->u8g2->setContrast(8);  // 0 > 255
  this->u8g2->setDrawColor(1);
  this->clear();
}

void Display::setConnectionInfo(String ip, String ssid) {
  this->ip = ip;
  this->ssid = ssid;
}

void Display::clear(int color) {
  // paints all pixels according to the desired target color
  for (uint8_t y = 0; y < 64; y++) {
    for (uint8_t x = 0; x < 128; x++) {
      this->u8g2->setDrawColor(color);
      this->u8g2->drawPixel(x, y);
    }
  }
  delay(100);
  this->u8g2->setDrawColor(color == 0 ? 1 : 0);
  this->u8g2->setFont(u8g2_font_6x13_te);
}

void Display::playSplashScreen() {
  // initial start screen

  this->initialize();

  // invert colors
  this->clear(1);

  this->u8g2->setFont(u8g2_font_nine_by_five_nbp_t_all);
  this->u8g2->setDrawColor(0);
  this->u8g2->drawStr(40, 53, AUTHOR_SIGNATURE.c_str());
  this->u8g2->sendBuffer();

  this->u8g2->setDrawColor(1);

  int n = 1;

  // animated splash
  for (int i = 128; i > 7; i = i - 18) {
    for (int j = 0; j < 18; j += 9) {
      this->u8g2->drawXBM(i - j - 11, 8, 128, 32, etktLogo);
      this->u8g2->sendBuffer();
    }
    auto character = Utility::utf8CharAt(STARTUP_MELODY, n);
    if (character != " ") {
      this->sound->play(character, 200);
    }
    n++;
  }

  // draw a box with subtractive color
  this->u8g2->setDrawColor(2);
  this->u8g2->drawBox(0, 0, 128, 64);
  this->u8g2->sendBuffer();

  sound->play(3000, 150);
}

void Display::renderConfig() {
  // screen for the wifi configuration mode

  this->clear();

  this->u8g2->setFont(u8g2_font_nine_by_five_nbp_t_all);
  this->u8g2->drawStr(15, 12, "WI-FI SETUP");
  this->u8g2->drawStr(3, 32, "Please, connect to");
  this->u8g2->drawStr(3, 47, "the \"E-TKT\" network...");

  this->u8g2->setFont(u8g2_font_open_iconic_all_1x_t);
  this->u8g2->drawGlyph(3, 12, 0x011a);

  this->u8g2->sendBuffer();
}

void Display::renderReset() {
  // screen for the wifi configuration reset confirmation

  this->clear();

  this->u8g2->setFont(u8g2_font_nine_by_five_nbp_t_all);
  this->u8g2->drawStr(15, 12, "WI-FI RESET");
  this->u8g2->drawStr(3, 32, "Connection cleared!");
  this->u8g2->drawStr(3, 47, "Release the button.");

  this->u8g2->setFont(u8g2_font_open_iconic_all_1x_t);
  this->u8g2->drawGlyph(3, 12, 0x00cd);

  this->u8g2->sendBuffer();
}

void Display::renderIdle() {
  // main screen with qr code, network and attributed ip

  this->clear();
  this->u8g2->setFont(u8g2_font_nine_by_five_nbp_t_all);

  uint8_t qrcodeData[qrcode_getBufferSize(this->QRcode_Version)];

  if (this->ip != "") {
    this->u8g2->setDrawColor(1);

    this->u8g2->drawStr(14, 15, "E-TKT");
    this->u8g2->setDrawColor(2);
    this->u8g2->drawFrame(3, 3, 50, 15);
    this->u8g2->setDrawColor(1);

    this->u8g2->drawStr(14, 31, "ready");

    String resizeSSID;
    if (this->ssid.length() > 8) {
      resizeSSID = this->ssid.substring(0, 7) + "...";
    } else {
      resizeSSID = this->ssid;
    }
    const char *d = resizeSSID.c_str();
    this->u8g2->drawStr(14, 46, d);

    const char *b = this->ip.c_str();
    this->u8g2->drawStr(3, 61, b);

    this->u8g2->setFont(u8g2_font_open_iconic_all_1x_t);
    this->u8g2->drawGlyph(3, 46, 0x00f8);
    this->u8g2->drawGlyph(3, 31, 0x0073);

    String ipFull = "http://" + this->ip;
    qrcode_initText(qrcode, qrcodeData, QRcode_Version, QRcode_ECC,
                    ipFull.c_str());

    // qr code background
    for (uint8_t y = 0; y < 64; y++) {
      for (uint8_t x = 0; x < 64; x++) {
        this->u8g2->setDrawColor(0);
        this->u8g2->drawPixel(x + 128 - 64, y);
      }
    }

    // setup the top right corner of the QRcode
    uint8_t x0 = 128 - 64 + 6;
    uint8_t y0 = 3;

    // display QRcode
    for (uint8_t y = 0; y < qrcode->size; y++) {
      for (uint8_t x = 0; x < qrcode->size; x++) {
        int newX = x0 + (x * 2);
        int newY = y0 + (y * 2);

        if (qrcode_getModule(qrcode, x, y)) {
          this->u8g2->setDrawColor(1);
          this->u8g2->drawBox(newX, newY, 2, 2);
        } else {
          this->u8g2->setDrawColor(0);
          this->u8g2->drawBox(newX, newY, 2, 2);
        }
      }
    }
  }
  this->u8g2->sendBuffer();
  delay(1000);
}

void Display::renderProgress(float progress, String label) {
  this->clear();

  // Show "⚙️ PRINTING" header.
  this->u8g2->setDrawColor(1);
  this->u8g2->setFont(u8g2_font_nine_by_five_nbp_t_all);
  this->u8g2->drawStr(15, 12, "PRINTING");
  this->u8g2->setFont(u8g2_font_open_iconic_all_1x_t);
  this->u8g2->drawGlyph(3, 12, 0x0081);

  auto labelLength = Utility::utf8Length(label);
  int progress_width = 0;
  int total_width = 0;

  // Do a pass thorugh the label characters to see how much horizontal
  // space is needed to render all the characters and how wide the completed
  // progress bar will be.
  for (int i = 0; i < labelLength; i++) {
    auto character = Utility::utf8CharAt(label, i);
    auto font = this->characters->getFont(character);
    total_width += font.width;
    if (i < progress) {
      progress_width += font.width;
    }
  }

  // Calculate the render offset, which keeps the currently printing location
  // visible if the label doesn't fit all on screen.
  int render_offset = 0;
  if (total_width > SCREEN_WIDTH) {
    // If the "progress" location is off screen, offset the rendered label
    // so the progress indicator is centered.
    if (progress_width > SCREEN_WIDTH / 2) {
      render_offset = progress_width - SCREEN_WIDTH / 2;
    }

    // If centering the progress location woudl cause the right side of the
    // label to render before the right edge of the screen then realign so
    // it does, simulating a scrolling box's bounds.
    if (total_width - render_offset < SCREEN_WIDTH) {
      render_offset = total_width - SCREEN_WIDTH;
    }
  }

  // Iterate through the label again, this time drawing it on screen.  For
  // simplicity's sake always draw the entire label (even if its of screen)
  // and just let the screen buffer clip the edges.
  int x_position = 0;
  const int y_position = 36;
  for (int i = 0; i < labelLength; i++) {
    auto character = Utility::utf8CharAt(label, i);
    auto font = this->characters->getFont(character);
    this->u8g2->setFont(font.font);
    auto characterX = x_position + font.width_offset - render_offset;
    auto characterY = y_position + font.height_offset;
    if (font.isSymbol()) {
      this->u8g2->drawGlyph(characterX, characterY, font.code);
    } else {
      this->u8g2->drawStr(characterX, characterY, character.c_str());
    }
    x_position += font.width;
  }

  if (progress_width > 0) {
    // Render an inverted-color rectangle over the completed characters.
    this->u8g2->setDrawColor(2);
    this->u8g2->drawBox(0 - render_offset, 21, progress_width - 1, 21);
  }

  // Draw a box around the text, which looks like the edges of a label.
  this->u8g2->setDrawColor(1);
  this->u8g2->drawFrame(0 - render_offset, 21, total_width, 22);

  // If needed, draw ellipses on the right side of the screen to indicate the
  // label continues.
  if (total_width - render_offset > SCREEN_WIDTH) {
    this->u8g2->setDrawColor(0);

    // Clear 14 pixels of space on the right side of the screen.
    this->u8g2->drawBox(SCREEN_WIDTH - 14, 21, 14, 22);

    // Draw a triplet of 2x2 pixel dots, "...", in the middle of the
    // text line.
    this->u8g2->setDrawColor(1);
    for (int i = 1; i <= 3; i++) {
      this->u8g2->drawBox(SCREEN_WIDTH - (i * 4) + 2, 31, 2, 2);
    }
  }

  // Print "XX%" at the bottom of the screen.
  String progressString = String((int)(100.0f * progress / labelLength)) + "%";
  this->u8g2->setDrawColor(1);
  this->u8g2->drawStr(6, 60, progressString.c_str());

  this->u8g2->sendBuffer();
}

void Display::renderFinished() {
  // screen with finish confirmation

  this->clear(1);
  this->u8g2->setDrawColor(0);
  this->u8g2->setFont(u8g2_font_nine_by_five_nbp_t_all);
  this->u8g2->drawStr(42, 37, "FINISHED!");

  this->u8g2->setFont(u8g2_font_open_iconic_all_1x_t);
  this->u8g2->drawGlyph(27, 37, 0x0073);
  this->u8g2->drawGlyph(90, 37, 0x0073);

  this->u8g2->sendBuffer();
}

void Display::renderCut() {
  // screen for manual cut mode

  this->clear(0);
  this->u8g2->setDrawColor(1);
  this->u8g2->setFont(u8g2_font_nine_by_five_nbp_t_all);
  this->u8g2->drawStr(44, 37, "CUTTING");

  this->u8g2->setFont(u8g2_font_open_iconic_all_1x_t);
  this->u8g2->drawGlyph(26, 37, 0x00f2);
  this->u8g2->drawGlyph(90, 37, 0x00f2);

  this->u8g2->sendBuffer();
}

void Display::renderFeed() {
  // screen for manual feed mode

  this->clear(0);
  this->u8g2->setDrawColor(1);
  this->u8g2->setFont(u8g2_font_nine_by_five_nbp_t_all);
  this->u8g2->drawStr(44, 37, "FEEDING");

  this->u8g2->setFont(u8g2_font_open_iconic_all_1x_t);
  this->u8g2->drawGlyph(26, 37, 0x006e);
  this->u8g2->drawGlyph(90, 37, 0x006e);

  this->u8g2->sendBuffer();
}

void Display::renderReel() {
  // screen for reeling mode

  this->clear(0);
  this->u8g2->setDrawColor(1);
  this->u8g2->setFont(u8g2_font_nine_by_five_nbp_t_all);
  this->u8g2->drawStr(44, 37, "REELING");

  this->u8g2->setFont(u8g2_font_open_iconic_all_1x_t);
  this->u8g2->drawGlyph(26, 37, 0x00d5);
  this->u8g2->drawGlyph(90, 37, 0x00d5);

  this->u8g2->sendBuffer();
}

void Display::renderTest(int a, int f) {
  // screen for settings test mode

  this->clear(0);
  this->u8g2->setDrawColor(1);
  this->u8g2->setFont(u8g2_font_nine_by_five_nbp_t_all);
  this->u8g2->drawStr(44, 37, "TESTING");

  this->u8g2->setFont(u8g2_font_open_iconic_all_1x_t);
  this->u8g2->drawGlyph(26, 37, 0x0073);
  this->u8g2->drawGlyph(90, 37, 0x0073);

  this->u8g2->sendBuffer();
}

void Display::renderSettings(int a, int f) {
  // screen for settings save mode

  this->clear(0);
  this->u8g2->setDrawColor(1);
  this->u8g2->setFont(u8g2_font_nine_by_five_nbp_t_all);
  this->u8g2->drawStr(47, 17, "SAVED!");

  String alignString = "ALIGN: ";
  alignString.concat(a);
  const char *alignChar = alignString.c_str();
  this->u8g2->drawStr(44, 37, alignChar);

  String forceString = "FORCE: ";
  forceString.concat(f);
  const char *forceChar = forceString.c_str();
  this->u8g2->drawStr(42, 57, forceChar);

  this->u8g2->setFont(u8g2_font_open_iconic_all_1x_t);
  this->u8g2->drawGlyph(33, 16, 0x0073);
  this->u8g2->drawGlyph(83, 16, 0x0073);

  this->u8g2->sendBuffer();
  delay(3000);
}

void Display::renderReboot() {
  // screen for imminent reboot

  this->clear();

  this->u8g2->setFont(u8g2_font_nine_by_five_nbp_t_all);
  this->u8g2->drawStr(38, 37, "REBOOTING...");

  this->u8g2->sendBuffer();
  delay(2000);
}