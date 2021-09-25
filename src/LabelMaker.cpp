#include <Arduino.h>
#include <AccelStepper.h>
#include <ESP32Servo.h>
// #include <movingAvg.h>
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <ESPAsyncWiFiManager.h>
#include <DNSServer.h>
#include "SPIFFS.h"

// HARDWARE ------------------------------------------------------------------------
#define CONFIG_ASYNC_TCP_RUNNING_CORE 0

#define MICROSTEP_Feed 8
#define MICROSTEP_Char 16

// home sensor
int sensorPin = A0;
int sensorState;

// stepper
const int stepsPerRevolutionFeed = 4076;
const int stepsPerRevolutionChar = 200 * MICROSTEP_Char;

AccelStepper stepperFeed(MICROSTEP_Feed, 2, 4, 3, 5);
AccelStepper stepperChar(1, 10, 11);
int fullCycle;
float stepsPerChar;

// servo
Servo myServo;
int restAngle = 55;
int peakAngle = 18;

// hall
int threshold = 128;
int currentCharPosition = -1;
int deltaPosition;
bool reverseCalibration;

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
int charHome = 21;
bool waitingLabel = false;

String headerCommand;
bool busy;
String operationStatus;

// temporary ********
int ledPin = 1;
String ledState;
//

// COMMUNICATION -------------------------------------------------------------------

// Create AsyncWebServer object on port 80
AsyncWebServer server(80);
DNSServer dns;

// Hardcoded login info
// const char *ssid = "";
// const char *password = "";
// const char *PARAM_MESSAGE = "message";

// ------------------------------------------------------------------------------------------------
// HARDWARE ------------------------------------------------------------------------

//new
void setHome()
{
	Serial.print("setHome() running on core ");
	Serial.println(xPortGetCoreID());

	Serial.println("0. homing");

	sensorState = analogRead(sensorPin);
	// Serial.print("reading a: ");
	// Serial.println(sensorState);

	if (sensorState < threshold)
	{
		Serial.println("over sensor");
		stepperChar.runToNewPosition(-stepsPerChar * 4);
		stepperChar.run();
	}

	sensorState = analogRead(sensorPin);
	// Serial.print("reading b: ");
	// Serial.println(sensorState);

	stepperChar.move(-stepsPerRevolutionChar * 1.5f);
	while (sensorState > threshold)
	{
		sensorState = analogRead(sensorPin);
		// Serial.println("run");
		stepperChar.run();
	}
	stepperChar.setCurrentPosition(0);

	sensorState = analogRead(sensorPin);
	// Serial.print("reading c: ");
	// Serial.println(sensorState);

	stepperChar.runToNewPosition(stepsPerChar * 0.75f);
	stepperChar.run();
	stepperChar.setCurrentPosition(0);
	currentCharPosition = charHome;

	delay(100);
}

//new TODO: NEGATIVE FEED
void feedLabel()
{
	Serial.println("3. feeding");

	stepperFeed.enableOutputs();
	stepperFeed.runToNewPosition(stepperFeed.currentPosition() - stepsPerRevolutionFeed / 8); // TODO adjust length
	stepperFeed.disableOutputs();

	// delay(10);
	
	Serial.println("3. feeding DONE");
}

//new
void pressLabel()
{

	Serial.println("2. pressing");

	for (int pos = restAngle; pos > peakAngle; pos--)
	{
		myServo.write(pos);
		delay(5);
		// delay(12);
	}
	delay(500);
	for (int pos = peakAngle; pos < restAngle; pos++)
	{
		myServo.write(pos);
		// delay(12);
		delay(5);
	}
	delay(500);

	
	Serial.println("2. pressing DONE");
}

//new
void goToCharacter(char c)
{
	Serial.print("1. roaming for ");
	Serial.println(c);

	// int backAdditional = 0;

	if (c == '0')
	{
		//Serial.println("exception ZERO");
		c = 'O';
	}
	else if (c == '1')
	{
		//Serial.println("exception ONE");
		c = 'I';
	}

	for (int i = 0; i < charQuantity; i++)
	{
		if (c == charSet[i])
		{
			deltaPosition = i - currentCharPosition;

			Serial.print("   deltaPosition: ");
			Serial.println(deltaPosition);

			currentCharPosition = i;

			// if (i > charHome)
			//   reverseCalibration = true;
			// else
			//   reverseCalibration = false;
		}
	}

	if (deltaPosition < 0)
	{
		deltaPosition += 43;
	}

	Serial.print("   corrected deltaPosition: ");
	Serial.println(deltaPosition);

	stepperChar.runToNewPosition(-stepsPerChar * deltaPosition);
	delay(10);

	if (c == '*')
	{
		pressLabel();
		pressLabel();
		pressLabel();
	}
	else
	{
		pressLabel();
	}
}

void cutLabel(bool directOrder)
{
	Serial.print("cutLabel() running on core ");
	Serial.println(xPortGetCoreID());

	if (directOrder)
		busy = true;

	Serial.println("cutting label");
	goToCharacter('*');

	if (directOrder)
		busy = false;
}

//new
void writeLabel(String label)
{
	Serial.print("writeLabel() running on core ");
	Serial.println(xPortGetCoreID());
	busy = true;

	// abcdefghijklmnopqrstuvwxyz23456789*
	Serial.println(label);

	int labelLength = label.length();

	feedLabel();
	for (int i = 0; i < labelLength; i++)
	{
		if (label[i] != ' ')
		{
			goToCharacter(label[i]);
		}
		delay(50);
		feedLabel();
		setHome();
	}

	if (labelLength < 6 && labelLength != 1)
	{
		int spaceDelta = 6 - labelLength;
		Serial.print("space delta: ");
		Serial.println(spaceDelta);
		for (int i = 0; i < spaceDelta; i++)
		{
			feedLabel();
		}
	}

	cutLabel(false);
	Serial.println("finished");
	busy = false;
	setHome();
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

String processor(const String parameter, const String value = "")
{
	Serial.print("parameter: ");
	Serial.print(parameter);
	Serial.print(value != "" ? ", value: " : "");
	Serial.println(value);

	String label = value;
	label.toUpperCase();

	if (parameter == "fw")
	{
		feedLabel();
	}
	else if (parameter == "rw")
	{
		feedLabel();
	}
	else if (parameter == "cut")
	{
		cutLabel(true);
	}
	else if (parameter == "tag" && label != "")
	{
		writeLabel(label);
		Serial.print(", value: ");
		Serial.println(label);
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
	Serial.println(WiFi.localIP());

	// Route for root / web page
	server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
			  {
				  request->send(SPIFFS, "/index.html", String(), false);
				  //   request->send(SPIFFS, "/index.html", String(), false, processor);
			  });

	server.on("/&", HTTP_GET, [](AsyncWebServerRequest *request)
			  {
				  int paramsNr = request->params();
				  String parameter;
				  String value;

				  for (int i = 0; i < paramsNr; i++)
				  {
					  AsyncWebParameter *p = request->getParam(i);
					  parameter = p->name();
					  value = p->value();
					  processor(parameter, value);
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
			  { request->send(SPIFFS, "/favicon.ico"), "image";});

	// Start server
	server.begin();
}

void configModeCallback(AsyncWiFiManager *myWiFiManager)
{
	Serial.println("Entered config mode");
	Serial.println(WiFi.softAPIP());
	//if you used auto generated SSID, print it
	Serial.println(myWiFiManager->getConfigPortalSSID());
}

void wifiManager()
{
	//Local intialization. Once its business is done, there is no need to keep it around
	AsyncWiFiManager wifiManager(&server, &dns);
	//reset settings - for testing
	// wifiManager.resetSettings();

	//set callback that gets called when connecting to previous WiFi fails, and enters Access Point mode
	wifiManager.setAPCallback(configModeCallback);

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

	//if you get here you have connected to the WiFi
	Serial.println("connected!");

	initialize();
}

// CORE
//new
void setup()
{
	Serial.begin(9600);

	pinMode(sensorPin, INPUT_PULLUP);

	stepperFeed.setMaxSpeed(500);
	stepperFeed.setAcceleration(500);
	stepperChar.setMaxSpeed(8000000);
	stepperChar.setAcceleration(800000);

	myServo.attach(12);
	myServo.write(restAngle);
	delay(10);

	Serial.println();

	stepsPerChar = (float)stepsPerRevolutionChar / charQuantity;

	Serial.print("setup() running on core ");
	Serial.println(xPortGetCoreID());

	//
	setHome();
	Serial.println("booting...");

	wifiManager();
}

void loop()
{
	// readSerial();
	delay(50);
}