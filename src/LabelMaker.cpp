// MIT License

// Copyright (c) 2022 Andrei Speridião

// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:

// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

//

// ~~~~~~ IMPORTANT: do not forget to upload the files in "data" folder via SPIFFS ~~~~~~

// libraries
#include <Arduino.h>
#include <AccelStepper.h>
#include <ESP32Servo.h>
#include <WiFi.h>
#include "esp_wifi.h"
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <ESPAsyncWiFiManager.h>
#include "SPIFFS.h"
#include <qrcode.h>
#include <U8g2lib.h>
#include <ESP32Tone.h>
#include <Preferences.h>

// extension files
#include "etktLogo.cpp" // etkt logo
#include "pitches.cpp"	// list of notes

// HARDWARE -----------------------------------------------------------------------

#define MICROSTEP_Feed 8
#define MICROSTEP_Char 16

// home sensor
#define sensorPin 34
int sensorState;
#define threshold 128
int currentCharPosition = -1;
int deltaPosition;

// wifi reset
#define wifiResetPin 13

// steppers
#define stepsPerRevolutionFeed 4076
const int stepsPerRevolutionChar = 200 * MICROSTEP_Char;

AccelStepper stepperFeed(MICROSTEP_Feed, 15, 2, 16, 4);
AccelStepper stepperChar(1, 32, 33);
#define enableCharStepper 25
float stepsPerChar;

// servo
Servo myServo;
#define servoPin 14
#define restAngle 50 // prev 55
int peakAngle;
// #define strongAngle 16 // 13
// #define lightAngle 20  // 17

#define targetAngle 22 // 17

// oled
U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, U8X8_PIN_NONE);
#define Lcd_X 128
#define Lcd_Y 64

// leds
#define ledFinish 5
#define ledChar 17

// buzzer
#define buzzerPin 27

// DEBUG --------------------------------------------------------------------------
#define do_cut true
#define do_press true
#define do_char true
#define do_feed true
#define do_sound true
#define do_serial true

// DATA ---------------------------------------------------------------------------

// char
#define charQuantity 43

// conversion table to prevent multichar error
// ♡ ... <
// ☆ ... >
// ♪ ... ~
// € ... |

char charSet[charQuantity] = {
	'$', '-', '.', '2', '3', '4', '5', '6', '7', '8',
	'9', '*', 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H',
	'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R',
	'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', '<', '>',
	'~', '|', '@'};

int etktNotes[8] = {
	44, 44, 16, 2, 31, 22, 31, 44};

int charNoteSet[charQuantity + 1] = {
	NOTE_A4, NOTE_AS4, NOTE_B4, NOTE_C5, NOTE_CS5, NOTE_D5, NOTE_DS5, NOTE_E5, NOTE_F5, NOTE_FS5,
	NOTE_G5, NOTE_GS5, NOTE_A5, NOTE_AS5, NOTE_B5, NOTE_C6, NOTE_CS6, NOTE_D6, NOTE_DS6, NOTE_E6,
	NOTE_F6, NOTE_FS6, NOTE_G6, NOTE_GS6, NOTE_A6, NOTE_AS6, NOTE_B6, NOTE_C7, NOTE_CS7, NOTE_D7,
	NOTE_DS7, NOTE_E7, NOTE_F7, NOTE_FS7, NOTE_G7, NOTE_GS7, NOTE_A7, NOTE_AS7, NOTE_B7, NOTE_C8,
	NOTE_CS8, NOTE_D8, NOTE_DS8, 0};

String labelString;
char prevChar = 'J';
int charHome = 21;
bool waitingLabel = false;

volatile bool busy = false;

String parameter = "";
String value = "";
TaskHandle_t processorTaskHandle = NULL;

Preferences preferences;
#define defaultAlign 5 // 1 to 9, 5 is the mid value
#define defaultForce 1 // 1 to 9
int alignFactor = defaultAlign;
int forceFactor = defaultForce;
int newAlign = defaultAlign;
int newForce = defaultForce;
String combinedSettings = "x";

// --------------------------------------------------------------------------------
// COMMUNICATION ------------------------------------------------------------------

// create AsyncWebServer object on port 80
AsyncWebServer server(80);
DNSServer dns;

String webProgress = " 0";

// qr code
const int QRcode_Version = 3; //  set the version (range 1->40)
const int QRcode_ECC = 2;	  //  set the Error Correction level (range 0-3) or symbolic (ECC_LOW, ECC_MEDIUM, ECC_QUARTILE and ECC_HIGH)
QRCode qrcode;				  //  create the QR code
String displaySSID = "";
String displayIP = "";

// --------------------------------------------------------------------------------
// LEDS ---------------------------------------------------------------------------

void lightFinished()
{
	bool state = true;
	for (int i = 0; i < 10; i++)
	{
		analogWrite(ledFinish, state * 128);
		state = !state;
		delay(100);
	}
	for (int i = 128; i >= 0; i--)
	{
		analogWrite(ledFinish, i);
		state = !state;
		delay(25);
	}

	webProgress = " 0";
}

void lightChar(float state)
{
	analogWrite(ledChar, state * 128);
}

// --------------------------------------------------------------------------------
// BUZZER -------------------------------------------------------------------------

void sound(int frequency = 2000, int duration = 1000)
{
	tone(buzzerPin, frequency, duration);
}

void labelMusic(String label)
{
	int length = label.length();

	for (int i = 0; i < length; i++)
	{
		for (int j = 0; j < charQuantity; j++)
		{
			if (label[i] == charSet[j])
			{
#ifdef do_serial
				Serial.println(charNoteSet[j]);
#endif
				tone(buzzerPin, charNoteSet[j], 50);
			}
		}
		delay(50);
	}
}

// --------------------------------------------------------------------------------
// DISPLAY ------------------------------------------------------------------------

void displayClear(int color = 0)
{
	// empty pixels
	for (uint8_t y = 0; y < 64; y++)
	{
		for (uint8_t x = 0; x < 128; x++)
		{
			u8g2.setDrawColor(color); // change 0 to make QR code with black background
			u8g2.drawPixel(x, y);
		}
	}
	delay(100); // transfer internal memory to the display

	u8g2.setDrawColor(color == 0 ? 1 : 0);
	u8g2.setFont(u8g2_font_6x13_te);
}

void displayInitialize()
{
	u8g2.begin();
	u8g2.clearBuffer();
	u8g2.setContrast(8); // set OLED brightness(0->255)
	displayClear();
	u8g2.setDrawColor(1);
}

void displaySplash()
{
	displayInitialize();

	// invert colors
	displayClear(1);

	u8g2.setFont(u8g2_font_nine_by_five_nbp_t_all);
	u8g2.setDrawColor(0);
	u8g2.drawStr(40, 53, "andrei.cc");
	u8g2.sendBuffer();

	u8g2.setDrawColor(1);

	int n = 1;

	// animated splash
	for (int i = 128; i > 7; i = i - 18)
	{
		for (int j = 0; j < 18; j += 9)
		{
			u8g2.drawXBM(i - j - 11, 8, 128, 32, etktLogo);
			u8g2.sendBuffer();
		}
		if (charNoteSet[etktNotes[n]] != 44)
		{
#ifdef do_sound
			sound(charNoteSet[etktNotes[n]], 200);
#else
			delay(200);
#endif
		}
		n++;
	}

	u8g2.setDrawColor(2);
	u8g2.drawBox(0, 0, 128, 64);
	u8g2.sendBuffer();

#ifdef do_sound
	sound(3000, 150);
#else
	delay(150);
#endif
}

void displayConfig()
{
	displayClear();

	u8g2.setFont(u8g2_font_nine_by_five_nbp_t_all);
	u8g2.drawStr(15, 12, "WI-FI SETUP");
	u8g2.drawStr(3, 32, "Please, connect to");
	u8g2.drawStr(3, 47, "the \"E-TKT\" network...");

	u8g2.setFont(u8g2_font_open_iconic_all_1x_t);
	u8g2.drawGlyph(3, 12, 0x011a); // connected

	u8g2.sendBuffer();
}

void displayReset()
{
	displayClear();

	u8g2.setFont(u8g2_font_nine_by_five_nbp_t_all);
	u8g2.drawStr(15, 12, "WI-FI RESET");
	u8g2.drawStr(3, 32, "Connection cleared!");
	u8g2.drawStr(3, 47, "Release the button.");

	u8g2.setFont(u8g2_font_open_iconic_all_1x_t);
	u8g2.drawGlyph(3, 12, 0x00cd);

	u8g2.sendBuffer();
}

void displayQRCode()
{
	displayClear();
	u8g2.setFont(u8g2_font_nine_by_five_nbp_t_all);

	uint8_t qrcodeData[qrcode_getBufferSize(QRcode_Version)];

	if (displayIP != "")
	{
		u8g2.setDrawColor(1);

		//--------------------------------------------

		u8g2.drawStr(14, 15, "E-TKT");
		u8g2.setDrawColor(2);
		u8g2.drawFrame(3, 3, 50, 15);
		u8g2.setDrawColor(1);

		u8g2.drawStr(14, 31, "ready");

		String resizeSSID;
		if (displaySSID.length() > 8)
		{
			resizeSSID = displaySSID.substring(0, 7) + "...";
		}
		else
		{
			resizeSSID = displaySSID;
		}
		const char *d = resizeSSID.c_str();
		u8g2.drawStr(14, 46, d); // connected

		const char *b = displayIP.c_str();
		u8g2.drawStr(3, 61, b);

		u8g2.setFont(u8g2_font_open_iconic_all_1x_t);
		u8g2.drawGlyph(3, 46, 0x00f8); // connected
		u8g2.drawGlyph(3, 31, 0x0073);

		String ipFull = "http://" + displayIP;
		const char *c = ipFull.c_str();
		qrcode_initText(&qrcode, qrcodeData, QRcode_Version, QRcode_ECC, c); // ARK address

		// qr code background
		for (uint8_t y = 0; y < 64; y++)
		{
			for (uint8_t x = 0; x < 64; x++)
			{
				u8g2.setDrawColor(0);
				u8g2.drawPixel(x + 128 - 64, y);
			}
		}

		//--------------------------------------------
		// setup the top right corner of the QRcode
		uint8_t x0 = 128 - 64 + 6;
		uint8_t y0 = 3; // 16 is the start of the blue portion OLED in the yellow/blue split 64x128 OLED

		//--------------------------------------------
		// display QRcode
		for (uint8_t y = 0; y < qrcode.size; y++)
		{
			for (uint8_t x = 0; x < qrcode.size; x++)
			{

				int newX = x0 + (x * 2);
				int newY = y0 + (y * 2);

				if (qrcode_getModule(&qrcode, x, y))
				{
					u8g2.setDrawColor(1);
					u8g2.drawBox(newX, newY, 2, 2);
				}
				else
				{
					u8g2.setDrawColor(0);
					u8g2.drawBox(newX, newY, 2, 2);
				}
			}
		}
	}
	u8g2.sendBuffer();
	delay(1000);
}

void displayProgress(float total, float actual, String label)
{
	displayClear();

	u8g2.setDrawColor(1);

	u8g2.setFont(u8g2_font_nine_by_five_nbp_t_all);
	u8g2.drawStr(15, 12, "PRINTING");
	u8g2.setFont(u8g2_font_open_iconic_all_1x_t);
	u8g2.drawGlyph(3, 12, 0x0081);

	u8g2.setFont(u8g2_font_6x13_te);
	const char *c = label.c_str();
	u8g2.drawStr(0, 36, c);

	u8g2.setDrawColor(2);
	u8g2.drawBox(0, 21, (actual)*6, 21);

	// u8g2.drawBox(0, 31, 128, 2);

	// u8g2.setDrawColor(0);
	// u8g2.drawBox(label.length() * 6, 5, 128, 21);

	float progress = actual / total;
	String progressString = String(progress * 95, 0);
	webProgress = progressString;
	progressString.concat("%");
	const char *p = progressString.c_str();
	u8g2.setDrawColor(1);
	u8g2.drawStr(6, 60, p);

	u8g2.sendBuffer();
}

void displayFinished()
{
	displayClear(1);
	u8g2.setDrawColor(0);
	u8g2.setFont(u8g2_font_nine_by_five_nbp_t_all);
	webProgress = "finished";
	u8g2.drawStr(42, 37, "FINISHED!");

	u8g2.setFont(u8g2_font_open_iconic_all_1x_t);
	u8g2.drawGlyph(27, 37, 0x0073);
	u8g2.drawGlyph(90, 37, 0x0073);

	u8g2.sendBuffer();
}

void displayCut()
{
	displayClear(0);
	u8g2.setDrawColor(1);
	u8g2.setFont(u8g2_font_nine_by_five_nbp_t_all);
	u8g2.drawStr(44, 37, "CUTTING");

	u8g2.setFont(u8g2_font_open_iconic_all_1x_t);
	u8g2.drawGlyph(26, 37, 0x00f2);
	u8g2.drawGlyph(90, 37, 0x00f2);

	u8g2.sendBuffer();
}

void displayFeed()
{
	displayClear(0);
	u8g2.setDrawColor(1);
	u8g2.setFont(u8g2_font_nine_by_five_nbp_t_all);
	u8g2.drawStr(44, 37, "FEEDING");

	u8g2.setFont(u8g2_font_open_iconic_all_1x_t);
	u8g2.drawGlyph(26, 37, 0x006e);
	u8g2.drawGlyph(90, 37, 0x006e);

	u8g2.sendBuffer();
}

void displayReel()
{
	displayClear(0);
	u8g2.setDrawColor(1);
	u8g2.setFont(u8g2_font_nine_by_five_nbp_t_all);
	u8g2.drawStr(44, 37, "REELING");

	u8g2.setFont(u8g2_font_open_iconic_all_1x_t);
	u8g2.drawGlyph(26, 37, 0x00d5);
	u8g2.drawGlyph(90, 37, 0x00d5);

	// u8g2.drawHLine(0, 32, 128);
	// u8g2.drawVLine(64, 0, 64);

	u8g2.sendBuffer();
}

void displayTest(int a, int f)
{
	displayClear(0);
	u8g2.setDrawColor(1);
	u8g2.setFont(u8g2_font_nine_by_five_nbp_t_all);
	u8g2.drawStr(44, 37, "TESTING");
	// u8g2.drawStr(44, 37, "ALIGN " + (char)a + " FORCE " + (char)f);

	u8g2.setFont(u8g2_font_open_iconic_all_1x_t);
	u8g2.drawGlyph(26, 37, 0x00d5); // SETTINGS LOGO ??
	u8g2.drawGlyph(90, 37, 0x00d5); // SETTINGS LOGO ??

	// u8g2.drawHLine(0, 32, 128);
	// u8g2.drawVLine(64, 0, 64);

	u8g2.sendBuffer();
}

void displaySettings(int a, int f)
{
	displayClear(0);
	u8g2.setDrawColor(1);
	u8g2.setFont(u8g2_font_nine_by_five_nbp_t_all);
	u8g2.drawStr(47, 17, "SAVED!");

	String alignString = "ALIGN: ";
	alignString.concat(a);
	const char *alignChar = alignString.c_str();
	u8g2.drawStr(44, 37, alignChar);

	String forceString = "FORCE: ";
	forceString.concat(f);
	const char *forceChar = forceString.c_str();
	u8g2.drawStr(42, 57, forceChar);

	u8g2.setFont(u8g2_font_open_iconic_all_1x_t);
	u8g2.drawGlyph(33, 16, 0x0073);
	u8g2.drawGlyph(83, 16, 0x0073);

	// u8g2.drawHLine(0, 32, 128);
	// u8g2.drawVLine(64, 0, 64);

	u8g2.sendBuffer();
	delay(3000);
}

void displayReboot()
{
	displayClear(0);

	u8g2.setFont(u8g2_font_nine_by_five_nbp_t_all);
	u8g2.drawStr(38, 37, "REBOOTING...");

	// u8g2.drawHLine(0, 32, 128);
	// u8g2.drawVLine(64, 0, 64);

	u8g2.sendBuffer();
	delay(2000);
}

// --------------------------------------------------------------------------------
// MECHANICS ----------------------------------------------------------------------

void setHome(int align = alignFactor)
{
	float a = (align - 5.0f) / 10.0f;

#ifdef do_serial

	Serial.print("align: ");
	Serial.println(align);

	Serial.print("homing with a = ");
	Serial.println(a);
#endif

	sensorState = analogRead(sensorPin);

	if (sensorState < threshold)
	{
		stepperChar.runToNewPosition(-stepsPerChar * 4);
		stepperChar.run();
	}
	sensorState = analogRead(sensorPin);

	stepperChar.move(-stepsPerRevolutionChar * 1.5f);
	while (sensorState > threshold)
	{
		sensorState = analogRead(sensorPin);
		stepperChar.run();
		delayMicroseconds(100); // TODO less intrusive way to avoid freeze?
	}
	stepperChar.setCurrentPosition(0);
	sensorState = analogRead(sensorPin);

	// stepperChar.runToNewPosition(-stepsPerChar * calibrateChar);
	stepperChar.runToNewPosition(-stepsPerChar + (stepsPerChar * a));
	stepperChar.run();
	stepperChar.setCurrentPosition(0);
	currentCharPosition = charHome;

	delay(100);
}

void feedLabel(int repeat = 1)
{
#ifdef do_serial
	Serial.print("feed ");
	Serial.print(repeat);
	Serial.println("x");
#endif

	stepperFeed.enableOutputs();
	delay(10);

	for (int i = 0; i < repeat; i++)
	{
		stepperFeed.runToNewPosition((stepperFeed.currentPosition() - stepsPerRevolutionFeed / 8) * 1);
		delay(10);
	}

	stepperFeed.disableOutputs();

	delay(20);
}

void pressLabel(bool strong = false, int force = forceFactor, bool slow = false)
{
#ifdef do_serial
	Serial.println("press");
#endif
	int delayFactor = 0;

	if (strong)
	{
		peakAngle = targetAngle + 9 - force;
		delayFactor = 2;
	}
	else
	{
		peakAngle = targetAngle + 9 - force;
		delayFactor = slow ? 100 : 0;
	}
	lightChar(1.0f);

	for (int pos = restAngle; pos >= peakAngle; pos--)
	{
		myServo.write(pos);
		delay(delayFactor);
	}
	for (int i = 0; i < 5; i++)
	{
		myServo.write(peakAngle);
		delay(50);
	}

	for (int pos = peakAngle; pos <= restAngle; pos++)
	{
		myServo.write(pos);
		delay(delayFactor);
	}
	for (int i = 0; i < 5; i++)
	{
		myServo.write(restAngle);
		delay(50);
	}

	lightChar(0.2f);
}

void goToCharacter(char c, int override = alignFactor)
{

#ifdef do_serial
	Serial.println("char");
#endif

	setHome(override);

	if (c == '0')
	{
		c = 'O';
	}
	else if (c == '1')
	{
		c = 'I';
	}

	for (int i = 0; i < charQuantity; i++)
	{
		if (c == charSet[i])
		{
			deltaPosition = i - currentCharPosition;
			currentCharPosition = i;
		}
	}

	if (deltaPosition < 0)
	{
		deltaPosition += 43;
	}

	stepperChar.runToNewPosition(-stepsPerChar * deltaPosition); // TODO FIX ALIGNMENT  + (c == '*' ? 0.25 : 0)

	delay(25);
}

void cutLabel()
{
	goToCharacter('*');

	for (int i = 0; i < 3; i++)
	{
		pressLabel(true);
	}
}

void writeLabel(String label)
{
	stepperChar.enableOutputs();
	myServo.write(restAngle);
	delay(500);

#ifdef do_serial
	Serial.print("print ");
	Serial.println(label);
#endif

	// all possible characters: $-.23456789*abcdefghijklmnopqrstuvwxyz♡☆♪€@

	int labelLength = label.length();

	setHome();
	lightChar(0.2f);

	for (int i = 0; i < labelLength; i++)
	{
		if (label[i] == '_')
		{
			label[i] = ' ';
		}
	}

	displayInitialize();
	displayProgress(labelLength, 0, label);

#ifdef do_sound
	labelMusic(label);
#else
	delay(200);
#endif

#ifdef do_feed
	feedLabel();
#else
	delay(500);
#endif

	for (int i = 0; i < labelLength; i++)
	{

		if (label[i] != ' ' && label[i] != '_' && label[i] != prevChar)
		{
#ifdef do_char
			goToCharacter(label[i]);
#else
			delay(500);
#endif
			prevChar = label[i];
		}

		if (label[i] != ' ' && label[i] != '_')
		{

#ifdef do_press
			pressLabel();
#else
			delay(500);
#endif
		}

#ifdef do_feed
		feedLabel();
#else
		delay(500);
#endif
		delay(500);

		displayProgress(labelLength, i + 1, label);
	}

	if (labelLength < 6 && labelLength != 1)
	{
		int spaceDelta = 6 - labelLength;
		for (int i = 0; i < spaceDelta; i++)
		{
#ifdef do_feed
			feedLabel();
#else
			delay(500);
#endif
		}
	}

#ifdef do_cut
	cutLabel();
#else
	delay(500);
#endif

	lightChar(0.0f);

	// reset server parameters
	parameter = "";
	value = "";
	myServo.write(restAngle);

	stepperChar.disableOutputs();

	displayFinished();
	lightFinished();
	busy = false;
	displayQRCode();
	vTaskDelete(processorTaskHandle);
}

void readSerial()
{
	if (!waitingLabel)
	{
		waitingLabel = true;
	}
	while (Serial.available())
	{
		labelString = Serial.readStringUntil('\n');
		waitingLabel = false;
	}
}

// --------------------------------------------------------------------------------
// DATA ---------------------------------------------------------------------------

void loadSettings()
{
	preferences.begin("calibration", false);

	// check if align and force values are stored in memory
	int a = preferences.getUInt("align", 0);
	if (a == 0)
	{
		preferences.putUInt("align", defaultAlign);
	}
	int f = preferences.getUInt("force", 0);
	if (f == 0)
	{
		preferences.putUInt("force", defaultForce);
	}

	alignFactor = preferences.getUInt("align", 0);
	forceFactor = preferences.getUInt("force", 0);
	preferences.end();

	combinedSettings = alignFactor;
	combinedSettings.concat(forceFactor);

#ifdef do_serial
	Serial.print("combined settings (align/force) ");
	Serial.println(combinedSettings);
#endif
}

void testSettings(int tempAlign, int tempForce, bool full = true)
{
#ifdef do_serial
	Serial.print("testing align ");
	Serial.print(tempAlign);
	Serial.print(" and force ");
	Serial.println(tempForce);
#endif

	stepperChar.enableOutputs();

	if (full)
	{
		// feedLabel(4);
		feedLabel(2);
		goToCharacter('E', tempAlign);
		pressLabel(false, tempForce, false);
		feedLabel();
		goToCharacter('-', tempAlign);
		pressLabel(false, tempForce, false);
		feedLabel();
		goToCharacter('T', tempAlign);
		pressLabel(false, tempForce, false);
		feedLabel();
		goToCharacter('K', tempAlign);
		pressLabel(false, tempForce, false);
		feedLabel();
		goToCharacter('T', tempAlign);
		pressLabel(false, tempForce, false);
		feedLabel(2);
		goToCharacter('*', tempAlign);
		pressLabel(true, tempForce, false);
		pressLabel(true, tempForce, false);
		pressLabel(true, tempForce, false);
	}
	else
	{
		goToCharacter('M', tempAlign);
		pressLabel(false, 1, true);
	}

	stepperChar.disableOutputs();
}

void saveSettings(int tempAlign, int tempForce)
{
	preferences.begin("calibration", false);

	preferences.putUInt("align", tempAlign);
	preferences.putUInt("force", tempForce);

	alignFactor = tempAlign;
	forceFactor = tempForce;

#ifdef do_serial
	Serial.print("saved align ");
	Serial.print(alignFactor);
	Serial.print(" and force ");
	Serial.println(forceFactor);
#endif

	preferences.end();
}

void interpretSettings()
{
	// interpret parameters
	String a = value.substring(0, 1);
	String f = value.substring(2, 3);
	newAlign = (int)a.toInt();
	newForce = (int)f.toInt();

#ifdef do_serial
	Serial.print("align: ");
	Serial.print(newAlign);
	Serial.print(", force: ");
	Serial.println(newForce);
#endif
}

void processor(void *parameters)
{
	for (;;)
	{
		if (parameter == "feed")
		{
			busy = true;

			displayInitialize();
			displayFeed();

			analogWrite(ledFinish, 32);
			myServo.write(restAngle);
			delay(500);

#ifdef do_feed
			feedLabel();
#else
			delay(500);
#endif

			parameter = "";
			value = "";
			stepperChar.disableOutputs();
			myServo.write(restAngle);

			busy = false;
			webProgress = "finished";
			delay(500);
			webProgress = " 0";
			analogWrite(ledFinish, 0);
			lightChar(0.0f);
			displayQRCode();
			vTaskDelete(processorTaskHandle);
		}
		else if (parameter == "reel")
		{
			busy = true;

			displayInitialize();
			displayReel();
			analogWrite(ledFinish, 32);
			myServo.write(restAngle);
			delay(500);

			for (int i = 0; i < 16; i++)
			{
#ifdef do_feed
				feedLabel();
#else
				delay(500);
#endif
			}

			parameter = "";
			value = "";
			stepperChar.disableOutputs();
			busy = false;
			webProgress = "finished";
			delay(500);
			webProgress = " 0";
			analogWrite(ledFinish, 0);
			lightChar(0.0f);
			displayQRCode();
			vTaskDelete(processorTaskHandle);
		}
		else if (parameter == "cut")
		{
			busy = true;

			displayInitialize();
			displayCut();

			lightChar(0.2f);
			stepperChar.enableOutputs();
			myServo.write(restAngle);
			delay(500);

#ifdef do_cut
			cutLabel();
#else
			delay(500);
#endif

			parameter = "";
			value = "";
			stepperChar.disableOutputs();
			busy = false;
			webProgress = "finished";
			delay(500);
			webProgress = " 0";
			lightChar(0.0f);
			displayQRCode();
			vTaskDelete(processorTaskHandle);
		}
		else if (parameter == "tag" && value != "")
		{
			busy = true;
			String label = value;
			label.toUpperCase();
			writeLabel(label);
			busy = false;
		}
		else if ((parameter == "testalign" || parameter == "testfull") && value != "")
		{
			busy = true;
			interpretSettings();

			displayInitialize();
			displayTest(newAlign, newForce);
			analogWrite(ledFinish, 32);

			bool testFull = false;
			if (parameter == "testfull")
			{
				testFull = true;
#ifdef do_serial
				Serial.print("testing full temporary settings / align: ");
				Serial.print(newAlign);
				Serial.print(" / force: ");
				Serial.println(newForce);
#endif
			}
			else if (parameter == "testalign")
			{
				testFull = false;
#ifdef do_serial
				Serial.print("testing align temporary settings / align: ");
				Serial.print(newAlign);
				Serial.print(" / force: ");
				Serial.println(newForce);
#endif
			}

			testSettings(newAlign, newForce, testFull);

			newAlign = 0;
			newForce = 0;
			value = "";

			webProgress = "finished";
			delay(500);
			webProgress = " 0";

			analogWrite(ledFinish, 0);
			lightChar(0.0f);
			displayQRCode();

			busy = false;
			vTaskDelete(processorTaskHandle);
		}
		else if (parameter == "save" && value != "")
		{
			busy = true;
			interpretSettings();

#ifdef do_serial
			Serial.println("saving settings");
#endif

			displayInitialize();
			displaySettings(newAlign, newForce);
			analogWrite(ledFinish, 32);

			saveSettings(newAlign, newForce);

			newAlign = 0;
			newForce = 0;
			value = "";

			displayReboot();

			webProgress = "finished";
			delay(500);
			webProgress = " 0";

			analogWrite(ledFinish, 0);
			lightChar(0.0f);
			busy = false;
			delay(500);
			ESP.restart();
		}
	}
}

// --------------------------------------------------------------------------------
// COMMUNICATION ------------------------------------------------------------------

void notFound(AsyncWebServerRequest *request)
{
	request->send(404, "text/plain", "Not found");
}

void initialize()
{
	// Initialize SPIFFS
	if (!SPIFFS.begin())
	{
		// Serial.println("An Error has occurred while mounting SPIFFS");
		return;
	}

	// Route for root / web page
	server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
			  { request->send(SPIFFS, "/index.html", String(), false); });

	server.on("/&", HTTP_GET, [](AsyncWebServerRequest *request)
			  {
				  int paramsNr = request->params();

				  for (int i = 0; i < paramsNr; i++)
				  {
					  AsyncWebParameter *p = request->getParam(i);
					  parameter = p->name();
					  value = p->value();

					  if (!busy)
					  {
						  xTaskCreatePinnedToCore(
							  processor,			/* function that implements the task */
							  "processorTask",		/* name of the task */
							  10000,				/* number of words to be allocated to use on task  */
							  NULL,					/* parameter to be input on the task (can be NULL) */
							  1,					/* priority for the task (0 to N) */
							  &processorTaskHandle, /* reference to the task (can be NULL) */
							  0);					/* core 0 */
					  }
					  else
					  {
#ifdef do_serial
						Serial.println("<< DENYING, BUSY >>");
#endif
					  }
				  }
				  request->send(SPIFFS, "/index.html", String(), false); });

	// route to load style.css file
	server.on("/style.css", HTTP_GET, [](AsyncWebServerRequest *request)
			  { request->send(SPIFFS, "/style.css", "text/css"); });

	// route to load script.js file
	server.on("/script.js", HTTP_GET, [](AsyncWebServerRequest *request)
			  { request->send(SPIFFS, "/script.js", "text/javascript"); });

	// route to load fonts
	server.on("/fontwhite.ttf", HTTP_GET, [](AsyncWebServerRequest *request)
			  { request->send(SPIFFS, "/fontwhite.ttf", "font"); });

	// route to favicon
	server.on("/favicon.ico", HTTP_GET, [](AsyncWebServerRequest *request)
			  { request->send(SPIFFS, "/favicon.ico", "image"); });

	// route to splash icon
	server.on("/splash.png", HTTP_GET, [](AsyncWebServerRequest *request)
			  { request->send(SPIFFS, "/splash.png", "image"); });

	// route to manifest file
	server.on("/manifest.json", HTTP_GET, [](AsyncWebServerRequest *request)
			  { request->send(SPIFFS, "/manifest.json", "image"); });

	// route to isometric image
	server.on("/iso.png", HTTP_GET, [](AsyncWebServerRequest *request)
			  { request->send(SPIFFS, "/iso.png", "image"); });

	// check printing status
	server.on("/status", HTTP_GET, [](AsyncWebServerRequest *request)
			  { request->send(200, "text/plane", webProgress); });

	// provide stored settings
	server.on("/settings", HTTP_GET, [](AsyncWebServerRequest *request)
			  { request->send(200, "text/plane", combinedSettings);  Serial.print("giving combinedSettings: ");  Serial.print(combinedSettings); });

	// start server
	server.begin();
}

void configModeCallback(AsyncWiFiManager *myWiFiManager)
{
	displayConfig();

#ifdef do_serial
	Serial.println(myWiFiManager->getConfigPortalSSID());
#endif
}

void clearWifiCredentials()
{
	// load the flash-saved configs
	wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
	esp_wifi_init(&cfg); // initiate and allocate wifi resources (does not matter if connection fails)
	delay(2000);		 // wait a bit
	if (esp_wifi_restore() != ESP_OK)
	{
#ifdef do_serial
		Serial.println("WiFi is not initialized by esp_wifi_init ");
#endif
	}
	else
	{
#ifdef do_serial
		Serial.println("WiFi Configurations Cleared!");
#endif
	}
	displayReset();
	delay(1500);
	esp_restart();
}

void wifiManager()
{
	// Local intialization. Once its business is done, there is no need to keep it around
	AsyncWiFiManager wifiManager(&server, &dns);

	bool wifiReset = digitalRead(wifiResetPin);
	if (!wifiReset)
	{
		clearWifiCredentials();
	}

	// set callback that gets called when connecting to previous WiFi fails, and enters Access Point mode
	wifiManager.setAPCallback(configModeCallback);

	wifiManager.setDebugOutput(false);

	// fetches ssid and pass and tries to connect
	// if it does not connect it starts an access point with the specified name
	// here  "AutoConnectAP"
	// and goes into a blocking loop awaiting configuration
	if (!wifiManager.autoConnect("E-TKT"))
	{
		// Serial.println("failed to connect and hit timeout");
		// reset and try again, or maybe put it to deep sleep
		ESP.restart();
		delay(1000);
	}

	// if (!MDNS.begin("e-tkt"))
	// {
	// 	// Serial.println("Error starting mDNS");
	// 	return;
	// }

	// if you get here you have connected to the WiFi
	// Serial.println("connected!");
	displayIP = WiFi.localIP().toString();
	// Serial.println(displayIP);
	displaySSID = WiFi.SSID();
	// Serial.println(displaySSID);

	displayQRCode();

	initialize();
}

// --------------------------------------------------------------------------------
// CORE ---------------------------------------------------------------------------
void setup()
{
#ifdef do_serial
	Serial.begin(115200);
	Serial.println("setup");
#endif

	loadSettings();

	pinMode(sensorPin, INPUT_PULLUP);
	pinMode(wifiResetPin, INPUT);
	pinMode(ledChar, OUTPUT);
	pinMode(ledFinish, OUTPUT);
	pinMode(enableCharStepper, OUTPUT);

	analogWrite(ledChar, 0);
	analogWrite(ledFinish, 0);

	stepperFeed.setMaxSpeed(1000000);
	stepperFeed.setAcceleration(1000); // 6000?

	digitalWrite(enableCharStepper, HIGH);
	stepperChar.setMaxSpeed(20000 * MICROSTEP_Char);
	stepperChar.setAcceleration(1000 * MICROSTEP_Char);
	stepsPerChar = (float)stepsPerRevolutionChar / charQuantity;
	stepperChar.setPinsInverted(true, false, true);
	stepperChar.setEnablePin(enableCharStepper);
	stepperChar.disableOutputs();

	myServo.attach(servoPin);
	myServo.write(restAngle);
	delay(100);

	displayInitialize();
	displayClear();
	displaySplash();

	wifiManager();
	delay(2000); // time for the task to start
}

void loop()
{
	// feedLabel();

	// digitalWrite(enableCharStepper, LOW);

	// stepperChar.setCurrentPosition(0);
	// stepperChar.runToNewPosition(2000);
	// stepperChar.run();
	// delay(200);

	// digitalWrite(enableCharStepper, HIGH);

	// pressLabel();

	// delay(500);

	// display tests

	// displaySplash();
	// delay(2000);
	// displayReset();
	// delay(2000);
	// displayConfig();
	// delay(2000);
	// displayIP = "192.168.255.255";
	// displaySSID = "Wifi Network";
	// displayQRCode();
	// delay(2000);
	// displayProgress(7, 5, " TESTE ");
	// delay(5000);

	// displayFinished();
	// delay(2000);
	// displayCut();
	// delay(2000);
	// displayFeed();
	// delay(2000);
	// displayReel();
	// delay(2000);

	// displaySettings(1, 2);
	// delay(2000);
	// displayReboot();
	// delay(2000);
}