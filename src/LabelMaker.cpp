#include <Arduino.h>
#include <AccelStepper.h>
#include <ESP32Servo.h>
// #include <movingAvg.h>
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <ESPAsyncWiFiManager.h>
#include <DNSServer.h>
#include <ESPmDNS.h>
#include "SPIFFS.h"

// HARDWARE ------------------------------------------------------------------------
// #define CONFIG_ASYNC_TCP_RUNNING_CORE 0

// mega
// sensorPin = A0
// stepperFeed = 2, 4, 3, 5
// stepperChar = 10, 11

// 34, 35, 36, 39 > input only, no pup/pdwn

// esp32
// sensorPin = GPIO34
// servo = GPIO14
// stepperFeed = GPIO16, GPIO4, GPIO2, GPIO15
// stepperChar = GPIO32, GPIO33
// pn532 = GPIO19, GPIO23, GPIO18, GPIOO5
// resetWifi = GPIO13

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

// stepper
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
bool busy;
String operationStatus;

String parameter = "";
String value = "";
TaskHandle_t processorTaskHandle = NULL;

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

	busy = false;
	setHome();

	// reset server parameters
	parameter = "";
	value = "";
	Serial.println("						finished");
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

// void processor(const String parameter, const String value = "")
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
		feedLabel();
	}
	else if (parameter == "rw")
	{
		feedLabel();
	}
	else if (parameter == "cut")
	{
		cutLabel();
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

					  xTaskCreatePinnedToCore(
						  processor,			/* função que implementa a tarefa */
						  "processorTask",		/* nome da tarefa */
						  10000,				/* número de palavras a serem alocadas para uso com a pilha da tarefa */
						  NULL,					/* parâmetro de entrada para a tarefa (pode ser NULL) */
						  1,					/* prioridade da tarefa (0 a N) */
						  &processorTaskHandle, /* referência para a tarefa (pode ser NULL) */
						  0);					/* core 0 */

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
	// TODO: button to delete wifi data

	//Local intialization. Once its business is done, there is no need to keep it around
	AsyncWiFiManager wifiManager(&server, &dns);
	//reset settings - for testing

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

	if (!MDNS.begin("etkt"))
	{
		Serial.println("Error starting mDNS");
		return;
	}

	//if you get here you have connected to the WiFi
	Serial.println("connected!");

	initialize();
}

// CORE
void setup()
{
	Serial.begin(9600);

	pinMode(sensorPin, INPUT_PULLUP);
	pinMode(wifiResetPin, INPUT_PULLDOWN);

	bool wifiReset = digitalRead(wifiResetPin);

	// Serial.print("wifi reset? ");
	// Serial.println(wifiReset);
	if (wifiReset)
	{
		Serial.println("<< wifi reset >>");
		// wifiManager.resetSettings();
		  WiFi.disconnect(true);
	}

	stepperFeed.setMaxSpeed(40000);
	stepperFeed.setAcceleration(6000);
	stepperChar.setMaxSpeed(2000 * MICROSTEP_Char);
	stepperChar.setAcceleration(2000 * MICROSTEP_Char);

	myServo.attach(servoPin);
	myServo.write(restAngle);
	delay(10);

	Serial.println();

	stepsPerChar = (float)stepsPerRevolutionChar / charQuantity;

	Serial.println("boot");
	wifiManager();
	delay(500); //tempo para a tarefa iniciar

	setHome();
}

void loop()
{
	// readSerial();
}