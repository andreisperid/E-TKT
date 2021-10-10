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
bool reverseCalibration;

// wifi reset
const int wifiResetPin = 13;

// steppers
const int stepsPerRevolutionFeed = 4076;
const int stepsPerRevolutionChar = 200 * MICROSTEP_Char;

AccelStepper stepperFeed(MICROSTEP_Feed, 15, 4, 2, 16);
AccelStepper stepperChar(1, 33, 32);
int fullCycle;
float stepsPerChar;

// servo
Servo myServo;
const int servoPin = 14;
int restAngle = 55;
int peakAngle = 18;

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
char charSet[charQuantity] = {
	' ', '-', '.', '2', '3', '4', '5', '6', '7', '8',
	'9', '*', 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H',
	'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R',
	'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', '+', '+',
	'+', '+', '+'};
String labelString;
char prevChar;
int charHome = 21;
bool waitingLabel = false;

String headerCommand;
bool busy = false;
String operationStatus;

String parameter = "";
String value = "";
TaskHandle_t processorTaskHandle = NULL;

// COMMUNICATION -------------------------------------------------------------------

// Create AsyncWebServer object on port 80
AsyncWebServer server(80);
DNSServer dns;

// qr code
const int QRcode_Version = 3; //  set the version (range 1->40)
const int QRcode_ECC = 0;	  //  set the Error Correction level (range 0-3) or symbolic (ECC_LOW, ECC_MEDIUM, ECC_QUARTILE and ECC_HIGH)
QRCode qrcode;				  // Create the QR code
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
	analogWrite(ledFinish, 128);
	delay(5000);
	for (int i = 128; i >= 0; i--)
	{
		analogWrite(ledFinish, i);
		state = !state;
		delay(25);
	}
}

void lightChar(bool state)
{
	analogWrite(ledChar, state * 128);
}

// DISPLAY -------------------------------------------------------------------------

void displayClear()
{
	//Empty pixels
	for (uint8_t y = 0; y < 32; y++)
	{
		for (uint8_t x = 0; x < 128; x++)
		{
			u8g2.setDrawColor(0); //change 0 to make QR code with black background
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
	for (int i = 128; i > 18; i--)
	{
		u8g2.setFont(u8g2_font_inr21_mf);
		u8g2.drawStr(i, 29, "E-TKT");
		u8g2.sendBuffer();
		delay(5);
		// animated splash}
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
	// u8g2.drawStr(0, 12, "Initializing..."); // write something to the internal memory
	// u8g2.sendBuffer();						// transfer internal memory to the display
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

		//--------------------------------------------
		// display
		// u8g2.setDrawColor(1);
		// u8g2.drawStr(0, 12, "E-TKT"); // write something to the internal memory

		// if (displaySSID != "")
		// {
		const char *d = displaySSID.c_str();
		u8g2.drawStr(11, 12, d); // connected
		u8g2.setFont(u8g2_font_open_iconic_all_1x_t);
		u8g2.drawGlyph(0, 12, 0x00f8); // connected
		// }
		// else
		// {
		// 	u8g2.drawStr(11, 12, "disconnected"); // disconnected
		// 	u8g2.setFont(u8g2_font_open_iconic_all_1x_t);
		// 	u8g2.drawGlyph(0, 12, 0x0057); // disconnected
		// }

		qrcode_initText(&qrcode, qrcodeData, QRcode_Version, QRcode_ECC, c); //ARK address

		// qr code background
		for (uint8_t y = 0; y < 31; y++)
		{
			for (uint8_t x = 0; x < 31; x++)
			{
				u8g2.setDrawColor(1); //change 0 to make QR code with black background
				u8g2.drawPixel(x + 127 - 32, y);
			}
		}

		//--------------------------------------------
		//setup the top right corner of the QRcode
		uint8_t x0 = 128 - 32;
		uint8_t y0 = 1; //16 is the start of the blue portion OLED in the yellow/blue split 64x128 OLED

		//--------------------------------------------
		//display QRcode
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
		// u8g2.sendBuffer();
	}
	u8g2.sendBuffer();
	delay(1000);
}

void displayProgress(float total, float actual, String label)
{
	float progress = actual / total;
	String progressString = String(progress * 95, 0);
	progressString.concat("%");
	const char *p = progressString.c_str();
	u8g2.drawStr(0, 12, p);
	// Serial.print("progress: ");
	// Serial.println(p);
	u8g2.setDrawColor(1);
	u8g2.drawHLine(0, 16, 128 - (progress * 128.00f));
	u8g2.drawHLine(0, 17, 128 - (progress * 128.00f));

	u8g2.setDrawColor(0);
	u8g2.drawHLine(128 - (progress * 128.00f), 16, 128);
	u8g2.drawHLine(128 - (progress * 128.00f), 17, 128);

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

	u8g2.setFont(u8g2_font_6x13_te);  // 9 pixel height
	u8g2.drawStr(0, 12, "Finished!"); // write something to the internal memory
	u8g2.sendBuffer();				  // transfer internal memory to the display
}

// HARDWARE ------------------------------------------------------------------------

void setHome()
{
	Serial.println("	home");

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

	delay(250);
}

// TODO: NEGATIVE FEED
void feedLabel()
{
	Serial.println("				feed");

	stepperFeed.enableOutputs();
	stepperFeed.runToNewPosition(stepperFeed.currentPosition() - stepsPerRevolutionFeed / 8); // TODO adjust length
	stepperFeed.disableOutputs();

	delay(50);

	// Serial.println("3. feeding DONE");
}

void pressLabel()
{
	Serial.println("			press");
	for (int pos = restAngle; pos > peakAngle; pos--)
	{
		myServo.write(pos);
		// delay(2);
	}
	delay(500);
	for (int pos = peakAngle; pos < restAngle; pos++)
	{
		myServo.write(pos);
		// delay(2);
	}
	delay(500);
	// Serial.println("2. pressing DONE");
}

void goToCharacter(char c)
{
	setHome();

	if (c != '*')
	{
		Serial.print("		\" ");
		Serial.print(c);
		Serial.println(" \"");
	}

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
	delay(10);

	// if (c != '*')
	// {
	// 	pressLabel();
	// }
}

void cutLabel()
{
	Serial.println("					cut");
	goToCharacter('*');

	for (int i = 0; i < 3; i++)
	{
		pressLabel();
	}
}

void writeLabel(String label)
{
	// abcdefghijklmnopqrstuvwxyz23456789*

	int labelLength = label.length();

	lightChar(true);
	displayInitialize();
	displayProgress(labelLength, 0, label);

	feedLabel();

	for (int i = 0; i < labelLength; i++)
	{
		if (label[i] != ' ' && label[i] != prevChar)
		{
			goToCharacter(label[i]);
			prevChar = label[i];
		}
		delay(50);
		pressLabel();
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

	// reset server parameters
	parameter = "";
	value = "";
	Serial.println("						finished");
	lightChar(false);
	displayFinished();
	lightFinished();

	busy = false;
	setHome();
	displayQRCode();
	vTaskDelete(processorTaskHandle);
}

void readSerial()
{
	if (!waitingLabel)
	{
		Serial.println("write label and hit enter");
		waitingLabel = true;
	}
	while (Serial.available())
	{
		labelString = Serial.readStringUntil('\n');
		Serial.print("TAG: ");
		Serial.println(labelString);

		// writeLabel(labelString);
		waitingLabel = false;
	}
}

// DATA ----------------------------------------------------------------------------

void processor(void *parameters)
{
	// Serial.print("parameter: ");
	Serial.print(parameter);

	String label = value;
	label.toUpperCase();

	Serial.print(label != "" ? ", value: " : "");
	Serial.println(label);

	if (parameter == "fw")
	{
		busy = true;
		feedLabel();
		busy = false;
	}
	else if (parameter == "rw")
	{
		busy = true;
		feedLabel();
		busy = false;
	}
	else if (parameter == "cut")
	{
		busy = true;
		cutLabel();
		busy = false;
	}
	else if (parameter == "tag" && label != "")
	{
		busy = true;
		// Serial.print(", value: ");
		// Serial.println(label);
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
		Serial.println("An Error has occurred while mounting SPIFFS");
		return;
	}

	// Print ESP32 Local IP Address
	// Serial.println(WiFi.localIP());

	// Route for root / web page
	server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
			  {
				  request->send(SPIFFS, "/index.html", String(), false);
				  //   request->send(SPIFFS, "/index.html", String(), false, processor);
			  });

	server.on("/&", HTTP_GET, [](AsyncWebServerRequest *request)
			  {
				  int paramsNr = request->params();
				  //   String parameter;
				  //   String value;

				  for (int i = 0; i < paramsNr; i++)
				  {
					  AsyncWebParameter *p = request->getParam(i);
					  parameter = p->name();
					  value = p->value();

					  if (!busy)
					  {
						  xTaskCreatePinnedToCore(
							  processor,			/* função que implementa a tarefa */
							  "processorTask",		/* nome da tarefa */
							  10000,				/* número de palavras a serem alocadas para uso com a pilha da tarefa */
							  NULL,					/* parâmetro de entrada para a tarefa (pode ser NULL) */
							  1,					/* prioridade da tarefa (0 a N) */
							  &processorTaskHandle, /* referência para a tarefa (pode ser NULL) */
							  0);					/* core 0 */
					  }
					  else
					  {
						  Serial.println("<< DENYING, BUSY >>");
					  }

					  //   processor(parameter, value);
				  }
				  request->send(SPIFFS, "/index.html", String(), false);
			  });

	// Route to load style.css file
	server.on("/style.css", HTTP_GET, [](AsyncWebServerRequest *request)
			  { request->send(SPIFFS, "/style.css", "text/css"); });

	// Route to load script.js file
	server.on("/script.js", HTTP_GET, [](AsyncWebServerRequest *request)
			  { request->send(SPIFFS, "/script.js", "text/javascript"); });

	// Route to load fonts
	server.on("/fontwhite.ttf", HTTP_GET, [](AsyncWebServerRequest *request)
			  { request->send(SPIFFS, "/fontwhite.ttf", "font"); });
	// Route to load fonts
	server.on("/fontblack.ttf", HTTP_GET, [](AsyncWebServerRequest *request)
			  { request->send(SPIFFS, "/fontblack.ttf", "font"); });

	// Route to favicon
	server.on("/favicon.ico", HTTP_GET, [](AsyncWebServerRequest *request)
			  { request->send(SPIFFS, "/favicon.ico", "image"); });

	// Route to splash icon
	server.on("/splash.png", HTTP_GET, [](AsyncWebServerRequest *request)
			  { request->send(SPIFFS, "/splash.png", "image"); });

	// Route to manifest file
	server.on("/manifest.json", HTTP_GET, [](AsyncWebServerRequest *request)
			  { request->send(SPIFFS, "/manifest.json", "image"); });

	// Route to webapp icon
	server.on("/icon192.png", HTTP_GET, [](AsyncWebServerRequest *request)
			  { request->send(SPIFFS, "/icon192.png", "image"); });

	// Route to webapp icon
	server.on("/icon512.png", HTTP_GET, [](AsyncWebServerRequest *request)
			  { request->send(SPIFFS, "/icon512.png", "image"); });

	// Start server
	server.begin();
}

void configModeCallback(AsyncWiFiManager *myWiFiManager)
{
	displayConfig();
	Serial.println("Entered config mode");
	Serial.println(WiFi.softAPIP());
	//if you used auto generated SSID, print it
	Serial.println(myWiFiManager->getConfigPortalSSID());
}

void clearWifiCredentials()
{
	//load the flash-saved configs
	wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
	esp_wifi_init(&cfg); //initiate and allocate wifi resources (does not matter if connection fails)
	delay(2000);		 //wait a bit
	if (esp_wifi_restore() != ESP_OK)
	{
		Serial.println("WiFi is not initialized by esp_wifi_init ");
	}
	else
	{
		Serial.println("WiFi Configurations Cleared!");
	}
	displayReset();
	delay(1500);
	esp_restart(); //just my reset configs routine...
}

void wifiManager()
{
	//Local intialization. Once its business is done, there is no need to keep it around
	AsyncWiFiManager wifiManager(&server, &dns);

	//reset settings - for testing
	bool wifiReset = digitalRead(wifiResetPin);

	// Serial.print("wifi reset? ");
	// Serial.println(wifiReset);

	if (wifiReset)
	{
		Serial.println("<< wifi reset >>");
		Serial.println();
		clearWifiCredentials();
		// wifiManager.resetSettings();
		// ESP.restart();
	}

	//set callback that gets called when connecting to previous WiFi fails, and enters Access Point mode
	wifiManager.setAPCallback(configModeCallback);

	// wifiManager.setDebugOutput(false);

	//fetches ssid and pass and tries to connect
	//if it does not connect it starts an access point with the specified name
	//here  "AutoConnectAP"
	//and goes into a blocking loop awaiting configuration
	if (!wifiManager.autoConnect("E-TKT"))
	{
		Serial.println("failed to connect and hit timeout");
		//reset and try again, or maybe put it to deep sleep
		ESP.restart();
		// ESP.reset();
		delay(1000);
	}

	if (!MDNS.begin("e-tkt"))
	{
		Serial.println("Error starting mDNS");
		return;
	}

	//if you get here you have connected to the WiFi
	Serial.println("connected!");
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
	Serial.begin(9600);

	pinMode(sensorPin, INPUT_PULLUP);
	pinMode(wifiResetPin, INPUT_PULLDOWN);
	pinMode(ledChar, OUTPUT);
	pinMode(ledFinish, OUTPUT);

	analogWrite(ledChar, 0);
	analogWrite(ledFinish, 0);

	stepperFeed.setMaxSpeed(40000);
	stepperFeed.setAcceleration(6000);
	stepperChar.setMaxSpeed(2000 * MICROSTEP_Char);
	stepperChar.setAcceleration(2000 * MICROSTEP_Char);

	myServo.attach(servoPin);
	myServo.write(restAngle);
	delay(10);

	stepsPerChar = (float)stepsPerRevolutionChar / charQuantity;

	displayInitialize();
	displayClear();
	displaySplash();

	Serial.println("boot");
	wifiManager();
	delay(500); //tempo para a tarefa iniciar

	setHome();
}

void loop()
{
	// readSerial();
}