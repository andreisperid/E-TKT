#include <Arduino.h>
#include <AccelStepper.h>
#include <ESP32Servo.h>
#include <movingAvg.h>
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <ESPAsyncWiFiManager.h>
#include <DNSServer.h>
#include "SPIFFS.h"

// HARDWARE ------------------------------------------------------------------------
#define HALFSTEP 8

#define CONFIG_ASYNC_TCP_RUNNING_CORE 0

// home sensor
int sensorPin = 36;	 //A0?
int emitterPin = 39; //A1?
int sensorState;
movingAvg sensorStateAvg(200);

// stepper
const int stepsPerRevolution = 4076;
AccelStepper stepperFeed(HALFSTEP, 2, 4, 3, 5);
AccelStepper stepperChar(HALFSTEP, 8, 10, 9, 11);
int fullCycle;
float stepsPerChar;

// servo
Servo myServo;
int restAngle = 22;
int peakAngle = 64;

// infrared
int minLight;
int maxLight;
int thresholdLight;
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

void setHome(bool calibrate)
{
	Serial.print("setHome() running on core ");
	Serial.println(xPortGetCoreID());

	digitalWrite(emitterPin, HIGH);
	if (calibrate)
	{
		stepperChar.setCurrentPosition(0);
		minLight = analogRead(sensorPin);
		maxLight = analogRead(sensorPin);
		Serial.println("calibrating...");

		stepperChar.moveTo(stepsPerRevolution * 1.1f);
		while (stepperChar.distanceToGo() > 0)
		{
			sensorState = analogRead(sensorPin);
			int averaged = sensorStateAvg.reading(sensorState);

			if (minLight != 0 && averaged < minLight)
			{
				minLight = averaged;
			}
			if (maxLight != 1023 && averaged > maxLight)
			{
				maxLight = averaged;
			}
			thresholdLight = (minLight + maxLight) / 2;

			//Serial.print("reading (c): ");
			//Serial.println(sensorState);
			//Serial.print("avg: ");
			//Serial.println(averaged);

			stepperChar.run();
		}
		//Serial.print("last reading: ");
		//Serial.println(sensorState);
		stepperChar.setCurrentPosition(0);
	}

	//Serial.print("threshold :");
	//Serial.print(thresholdLight);
	//Serial.print(" << calibrated >> minLight :");
	//Serial.print(minLight);
	//Serial.print(" / maxLight: ");
	//Serial.println(maxLight);

	//Serial.println("calling home...");
	delay(200);

	int targetPosition = stepsPerRevolution * 2;

	if (reverseCalibration)
	{
		targetPosition *= -1;
	}

	stepperChar.moveTo(targetPosition);
	sensorState = analogRead(sensorPin);

	//Serial.print("sensor ");
	//Serial.println(sensorState);
	sensorStateAvg.reset();
	int averaged = sensorStateAvg.reading(sensorState);

	while (averaged < thresholdLight)
	{
		sensorState = analogRead(sensorPin);
		averaged = sensorStateAvg.reading(sensorState);
		stepperChar.run();
		//if (sensorState > thresholdLight) Serial.println(sensorState);
	}
	stepperChar.setCurrentPosition(0);

	//Serial.print("last reading: ");
	//Serial.println(sensorState);
	currentCharPosition = charHome;
	if (reverseCalibration)
	{
		//Serial.println("<< reverse");
		stepperChar.runToNewPosition(-stepsPerChar);
		stepperChar.setCurrentPosition(0);
		reverseCalibration = false;
	}
	else
	{
		//Serial.println(">> forward");
	}

	//Serial.print("at home, position ");
	//Serial.println(currentCharPosition);

	digitalWrite(emitterPin, LOW);
	delay(500);
}

void feedLabel(bool directOrder, bool forward = true)
{
	Serial.print("feedLabel() running on core ");
	Serial.println(xPortGetCoreID());
	if (directOrder)
		busy = true;
	Serial.println("feeding label");
	stepperFeed.runToNewPosition(forward ? 1 : -1 * (stepperChar.currentPosition() - stepsPerRevolution / 2));
	delay(200);
	if (directOrder)
		busy = false;
}

void pressLabel()
{
	Serial.print("pressLabel() running on core ");
	Serial.println(xPortGetCoreID());
	for (int pos = restAngle; pos < peakAngle; pos++)
	{
		myServo.write(pos);
		delay(12);
	}
	delay(500);
	for (int pos = peakAngle; pos > restAngle; pos--)
	{
		myServo.write(pos);
		delay(12);
	}
}

void goToCharacter(char c)
{
	Serial.print("goToCharacter() running on core ");
	Serial.println(xPortGetCoreID());
	int backAdditional = 0;
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

			if (deltaPosition < 0)
				backAdditional = -stepsPerChar / 2;
			else if (deltaPosition > 0)
				backAdditional = stepsPerChar / 2;
			/*
			Serial.print("char ");
			Serial.print(c);
			Serial.print(" is on position ");
			Serial.print(i);
			Serial.print(" ( delta ");
			Serial.print(deltaPosition);
			Serial.println(")");
			*/
			currentCharPosition = i;

			if (i > charHome)
				reverseCalibration = true;
			else
				reverseCalibration = false;
		}
	}

	stepperChar.runToNewPosition(stepsPerChar * deltaPosition + backAdditional);
	delay(250);
	pressLabel();
	feedLabel(false);
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

void writeLabel(String label)
{
	Serial.print("writeLabel() running on core ");
	Serial.println(xPortGetCoreID());
	busy = true;
	feedLabel(false);
	for (int i = 0; i < label.length(); i++)
	{
		goToCharacter(label[i]);
		setHome(false);
	}
	cutLabel(false);
	Serial.println("finished");
	busy = false;
	setHome(false);
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

		writeLabel(labelString);
		waitingLabel = false;
	}
}

// DATA ----------------------------------------------------------------------------

String processor(const String parameter, const String value = "")
{
	Serial.print("parameter: ");
	Serial.print(parameter);
	Serial.print(value !="" ? ", value: " : "");
	Serial.println(value);

	// 	if (parameter == "fw")
	// 	{
	// 		feedLabel(true);
	// 	}
	// 	else if (parameter == "rw")
	// 	{
	// 		feedLabel(true, false);
	// 	}
	// 	else if (parameter == "cut")
	// 	{
	// 		cutLabel(true);
	// 	}
	// 	else if (parameter == "tag" && value != "")
	// 	{
	// 		writeLabel(value);
	// 		Serial.print(", value: ");
	// 		Serial.println(value);
	// 	}
}

String processorOld(const String &var)
{
	Serial.println(var);
	if (var == "TAG")
	{
		if (digitalRead(ledPin))
		{
			ledState = "ON";
		}
		else
		{
			ledState = "OFF";
		}
		Serial.print(ledState);
		return ledState;
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
			  { request->send(SPIFFS, "/favicon.ico"), "image"; });

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
void setup()
{
	Serial.begin(9600);

	Serial.print("setup() running on core ");
	Serial.println(xPortGetCoreID());

	pinMode(sensorPin, INPUT);
	pinMode(emitterPin, OUTPUT);

	stepperFeed.setMaxSpeed(1000.0);
	stepperFeed.setAcceleration(16000.0);
	stepperFeed.setSpeed(200);
	stepperChar.setMaxSpeed(1000.0);
	stepperChar.setAcceleration(16000.0);
	stepperChar.setSpeed(200);

	myServo.attach(12);
	myServo.write(restAngle);
	delay(100);

	Serial.println();
	//stepsPerChar = 100;
	//stepsPerChar = 95;
	stepsPerChar = (float)stepsPerRevolution / charQuantity;
	//Serial.print("stepsPerChar: ");
	//Serial.println(stepsPerChar);
	sensorStateAvg.begin();

	//
	setHome(true);
	Serial.println("booting...");

	wifiManager();
}

void loop()
{
	// readSerial();
	// delay(10);
}