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

// ~~~~~~IMPORTANT: do not forget to upload the files in "data" folder via SPIFFS

#include <Arduino.h>
#include <AccelStepper.h>
#include <ESP32Servo.h>
#include <WiFi.h>
#include "esp_wifi.h"
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <ESPAsyncWiFiManager.h>
#include <DNSServer.h>
#include <ESPmDNS.h>
#include "SPIFFS.h"
#include <qrcode.h>
#include <U8g2lib.h>


// HARDWARE ------------------------------------------------------------------------

#define MICROSTEP_Feed 8
#define MICROSTEP_Char 16

// home sensor
int sensorPin = 34;
int sensorState;
int threshold = 128;
int currentCharPosition = -1;
int deltaPosition;

// wifi reset
const int wifiResetPin = 13;

// steppers
const int stepsPerRevolutionFeed = 4076;
const int stepsPerRevolutionChar = 200 * MICROSTEP_Char;

AccelStepper stepperFeed(MICROSTEP_Feed, 15, 4, 2, 16);
AccelStepper stepperChar(1, 33, 32);
#define enableCharStepper 25
float stepsPerChar;

// servo
Servo myServo;
const int servoPin = 14;
int restAngle = 55;
int peakAngle = 20;
#define strongAngle 13
#define lightAngle 17

// oled
U8G2_SSD1306_128X32_UNIVISION_F_HW_I2C u8g2(U8G2_R0, U8X8_PIN_NONE);
#define Lcd_X 128
#define Lcd_Y 64

// leds
#define ledFinish 5
#define ledChar 17

// DATA ----------------------------------------------------------------------------

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

String labelString;
char prevChar = 'J';
int charHome = 21;
bool waitingLabel = false;

bool busy = false;

String parameter = "";
String value = "";
TaskHandle_t processorTaskHandle = NULL;

// COMMUNICATION -------------------------------------------------------------------

// create AsyncWebServer object on port 80
AsyncWebServer server(80);
DNSServer dns;

String webProgress = " 0";

// qr code
const int QRcode_Version = 3; //  set the version (range 1->40)
const int QRcode_ECC = 0;	  //  set the Error Correction level (range 0-3) or symbolic (ECC_LOW, ECC_MEDIUM, ECC_QUARTILE and ECC_HIGH)
QRCode qrcode;				  //  create the QR code
String displaySSID = "";
String displayIP = "";

// ------------------------------------------------------------------------------------------------
// LEDS -------------------------------------------------------------------------

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

// DISPLAY -------------------------------------------------------------------------

void displayClear()
{
	// empty pixels
	for (uint8_t y = 0; y < 32; y++)
	{
		for (uint8_t x = 0; x < 128; x++)
		{
			u8g2.setDrawColor(0); // change 0 to make QR code with black background
			u8g2.drawPixel(x, y);
		}
	}
	delay(100); // transfer internal memory to the display
}

void displayInitialize()
{
	u8g2.begin();
	u8g2.clearBuffer();
	u8g2.setFlipMode(1);
	u8g2.setContrast(64); // set OLED brightness(0->255)
	displayClear();

	u8g2.setFont(u8g2_font_6x13_te); // 9 pixel height
	u8g2.setDrawColor(1);
}

void displaySplash()
{
	displayInitialize();

	// animated splash
	for (int i = 128; i > 18; i = i - 2)
	{
		u8g2.setFont(u8g2_font_inr21_mf);
		u8g2.drawStr(i, 29, "E-TKT");
		u8g2.sendBuffer();
		delay(1);		
	}
}

void displayConfig()
{
	displayInitialize();
	u8g2.setFont(u8g2_font_6x13_te);
	u8g2.drawStr(0, 12, "Connect to the");
	u8g2.drawStr(0, 29, "\"E-TKT\" wifi network");
	u8g2.sendBuffer();
}

void displayReset()
{
	displayInitialize();
	u8g2.setFont(u8g2_font_6x13_te);
	u8g2.drawStr(0, 12, "Connection cleared");
	u8g2.drawStr(0, 29, "Release button");
	u8g2.sendBuffer();
}

void displayQRCode()
{
	displayInitialize();
	displayClear();

	uint8_t qrcodeData[qrcode_getBufferSize(QRcode_Version)];

	if (displayIP != "")
	{
		u8g2.setDrawColor(1);

		//--------------------------------------------
		const char *b = displayIP.c_str();
		u8g2.drawStr(0, 31, b); // write something to the internal memory

		String displayIPfull = "http://" + displayIP;
		const char *c = displayIPfull.c_str();

		const char *d = displaySSID.c_str();
		u8g2.drawStr(11, 12, d); // connected
		u8g2.setFont(u8g2_font_open_iconic_all_1x_t);
		u8g2.drawGlyph(0, 12, 0x00f8); // connected

		qrcode_initText(&qrcode, qrcodeData, QRcode_Version, QRcode_ECC, c); // ARK address

		// qr code background
		for (uint8_t y = 0; y < 31; y++)
		{
			for (uint8_t x = 0; x < 31; x++)
			{
				u8g2.setDrawColor(1); // change 0 to make QR code with black background
				u8g2.drawPixel(x + 127 - 32, y);
			}
		}

		//--------------------------------------------		
		// setup the top right corner of the QRcode
		uint8_t x0 = 128 - 32;
		uint8_t y0 = 1; // 16 is the start of the blue portion OLED in the yellow/blue split 64x128 OLED

		//--------------------------------------------
		// display QRcode
		for (uint8_t y = 0; y < qrcode.size; y++)
		{
			for (uint8_t x = 0; x < qrcode.size; x++)
			{

				if (qrcode_getModule(&qrcode, x, y) == 0)
				{
					u8g2.setDrawColor(1);
					u8g2.drawPixel(x0 + x, y0 + y);
				}
				else
				{
					u8g2.setDrawColor(0);
					u8g2.drawPixel(x0 + x, y0 + y);
				}
			}
		}
	}
	u8g2.sendBuffer();
	delay(1000);
}

void displayProgress(float total, float actual, String label)
{
	float progress = actual / total;
	String progressString = String(progress * 95, 0);
	webProgress = progressString;
	progressString.concat("%");
	const char *p = progressString.c_str();
	u8g2.drawStr(0, 12, p);

	u8g2.setDrawColor(0);
	u8g2.drawHLine(0, 16, 128);
	u8g2.drawHLine(0, 17, 128);

	u8g2.setDrawColor(1);
	u8g2.drawHLine(0, 16, (total - actual) * 6);
	u8g2.drawHLine(0, 17, (total - actual) * 6);

	u8g2.setDrawColor(1);
	u8g2.drawHLine(0, 16, 5);
	u8g2.drawHLine(0, 17, 5);

	u8g2.setDrawColor(1);
	const char *c = label.c_str();
	u8g2.drawStr(0, 31, c);
	u8g2.sendBuffer();
}

void displayFinished()
{
	displayInitialize();
	webProgress = "finished";

	u8g2.setFont(u8g2_font_9x15_te);   // 9 pixel height
	u8g2.drawStr(24, 24, "Finished!"); // write something to the internal memory
	u8g2.sendBuffer();				   // transfer internal memory to the display
}

// HARDWARE ------------------------------------------------------------------------

void setHome()
{
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
	}
	stepperChar.setCurrentPosition(0);
	sensorState = analogRead(sensorPin);

	stepperChar.runToNewPosition(stepsPerChar * 0.75f);
	stepperChar.run();
	stepperChar.setCurrentPosition(0);
	currentCharPosition = charHome;

	delay(100);
}

void feedLabel()
{
	stepperFeed.enableOutputs();
	delay(10);
	stepperFeed.runToNewPosition(stepperFeed.currentPosition() - stepsPerRevolutionFeed / 8);
	delay(10);
	stepperFeed.disableOutputs();

	delay(20);
}

void pressLabel(bool strong = false)
{
	int delayFactor;

	if (strong)
	{
		peakAngle = strongAngle;
		delayFactor = 2;
	}
	else
	{
		peakAngle = lightAngle;
		delayFactor = 1;
	}

	lightChar(1.0f);

	for (int pos = restAngle; pos > peakAngle; pos--)
	{
		myServo.write(pos);
		delay(delayFactor);
	}
	myServo.write(peakAngle);
	delay(50);
	myServo.write(peakAngle);
	delay(50);
	myServo.write(peakAngle);
	delay(50);
	myServo.write(peakAngle);
	delay(50);
	myServo.write(peakAngle);
	delay(50);

	for (int pos = peakAngle; pos < restAngle; pos++)
	{
		myServo.write(pos);
		delay(delayFactor);
	}
	myServo.write(restAngle);
	delay(50);
	myServo.write(restAngle);
	delay(50);
	myServo.write(restAngle);
	delay(50);
	myServo.write(restAngle);
	delay(50);
	myServo.write(restAngle);
	delay(50);

	lightChar(0.2f);
}

void goToCharacter(char c)
{
	setHome();

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

	stepperChar.runToNewPosition(-stepsPerChar * deltaPosition);
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
	digitalWrite(enableCharStepper, LOW);
	myServo.write(restAngle);
	delay(500);

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

	feedLabel();

	for (int i = 0; i < labelLength; i++)
	{

		if (label[i] != ' ' && label[i] != '_' && label[i] != prevChar)
		{
			goToCharacter(label[i]);
			prevChar = label[i];
		}
		// delay(100);

		if (label[i] != ' ' && label[i] != '_')
		{
			pressLabel();
		}

		feedLabel();

		displayProgress(labelLength, i + 1, label);
	}

	if (labelLength < 6 && labelLength != 1)
	{
		int spaceDelta = 6 - labelLength;
		for (int i = 0; i < spaceDelta; i++)
		{
			feedLabel();
		}
	}
	cutLabel();

	lightChar(0.0f);

	// reset server parameters
	parameter = "";
	value = "";
	digitalWrite(enableCharStepper, HIGH);

	myServo.write(restAngle);

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

// DATA ----------------------------------------------------------------------------

void processor(void *parameters)
{
	String label = value;
	label.toUpperCase();

	if (parameter == "feed" && label == "")
	{
		busy = true;
		analogWrite(ledFinish, 32);
		digitalWrite(enableCharStepper, LOW);
		myServo.write(restAngle);
		delay(500);

		feedLabel();

		parameter = "";
		value = "";
		digitalWrite(enableCharStepper, HIGH);
		myServo.write(restAngle);

		busy = false;
		webProgress = "finished";
		delay(500);
		webProgress = " 0";
		analogWrite(ledFinish, 0);
		lightChar(0.0f);
		vTaskDelete(processorTaskHandle);
	}
	else if (parameter == "reel" && label == "")
	{
		busy = true;
		analogWrite(ledFinish, 32);
		digitalWrite(enableCharStepper, LOW);
		myServo.write(restAngle);
		delay(500);

		for (int i = 0; i < 16; i++)
		{
			feedLabel();
		}

		parameter = "";
		value = "";
		digitalWrite(enableCharStepper, HIGH);
		busy = false;
		webProgress = "finished";
		delay(500);
		webProgress = " 0";
		analogWrite(ledFinish, 0);
		lightChar(0.0f);
		vTaskDelete(processorTaskHandle);
	}
	else if (parameter == "cut" && label == "")
	{
		busy = true;
		lightChar(0.2f);
		digitalWrite(enableCharStepper, LOW);
		myServo.write(restAngle);
		delay(500);

		cutLabel();

		parameter = "";
		value = "";
		digitalWrite(enableCharStepper, HIGH);
		busy = false;
		webProgress = "finished";
		delay(500);
		webProgress = " 0";
		lightChar(0.0f);
		vTaskDelete(processorTaskHandle);
	}
	else if (parameter == "tag" && label != "")
	{
		busy = true;
		writeLabel(label);
		busy = false;
	}
}

// COMMUNICATION -------------------------------------------------------------------
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
						  //   Serial.println("<< DENYING, BUSY >>");
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
	// route to load fonts
	server.on("/fontblack.ttf", HTTP_GET, [](AsyncWebServerRequest *request)
			  { request->send(SPIFFS, "/fontblack.ttf", "font"); });

	// route to favicon
	server.on("/favicon.ico", HTTP_GET, [](AsyncWebServerRequest *request)
			  { request->send(SPIFFS, "/favicon.ico", "image"); });

	// route to splash icon
	server.on("/splash.png", HTTP_GET, [](AsyncWebServerRequest *request)
			  { request->send(SPIFFS, "/splash.png", "image"); });

	// route to manifest file
	server.on("/manifest.json", HTTP_GET, [](AsyncWebServerRequest *request)
			  { request->send(SPIFFS, "/manifest.json", "image"); });

	// route to webapp icon
	server.on("/icon192.png", HTTP_GET, [](AsyncWebServerRequest *request)
			  { request->send(SPIFFS, "/icon192.png", "image"); });

	// route to webapp icon
	server.on("/icon512.png", HTTP_GET, [](AsyncWebServerRequest *request)
			  { request->send(SPIFFS, "/icon512.png", "image"); });

	// check printing status
	server.on("/status", HTTP_GET, [](AsyncWebServerRequest *request)
			  { request->send(200, "text/plane", webProgress); });

	// start server
	server.begin();
}

void configModeCallback(AsyncWiFiManager *myWiFiManager)
{
	displayConfig();
	Serial.println(myWiFiManager->getConfigPortalSSID());
}

void clearWifiCredentials()
{
	// load the flash-saved configs
	wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
	esp_wifi_init(&cfg); // initiate and allocate wifi resources (does not matter if connection fails)
	delay(2000);		 // wait a bit
	if (esp_wifi_restore() != ESP_OK)
	{
		// Serial.println("WiFi is not initialized by esp_wifi_init ");
	}
	else
	{
		// Serial.println("WiFi Configurations Cleared!");
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

	if (wifiReset)
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

	if (!MDNS.begin("e-tkt"))
	{
		// Serial.println("Error starting mDNS");
		return;
	}

	// if you get here you have connected to the WiFi
	// Serial.println("connected!");
	displayIP = WiFi.localIP().toString();
	// Serial.println(displayIP);
	displaySSID = WiFi.SSID();
	// Serial.println(displaySSID);

	displayQRCode();

	initialize();
}

// CORE
void setup()
{
	// Serial.begin(9600);

	pinMode(sensorPin, INPUT_PULLUP);
	pinMode(wifiResetPin, INPUT_PULLDOWN);
	pinMode(ledChar, OUTPUT);
	pinMode(ledFinish, OUTPUT);
	pinMode(enableCharStepper, OUTPUT);

	analogWrite(ledChar, 0);
	analogWrite(ledFinish, 0);

	digitalWrite(enableCharStepper, HIGH);

	stepperFeed.setMaxSpeed(100000);
	stepperFeed.setAcceleration(3000);
	stepperChar.setMaxSpeed(300 * MICROSTEP_Char);
	stepperChar.setAcceleration(10000 * MICROSTEP_Char);

	stepsPerChar = (float)stepsPerRevolutionChar / charQuantity;

	displayInitialize();
	displayClear();
	displaySplash();

	wifiManager();
	delay(500); // time for the task to start

	myServo.attach(servoPin);
	myServo.write(restAngle);
	delay(500);
}

void loop()
{
}