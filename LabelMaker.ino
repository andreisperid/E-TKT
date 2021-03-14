#include <AccelStepper.h>
#include <Servo.h>
#include <movingAvg.h> 

#define HALFSTEP 8

// home sensor
int sensorPin = A0;
int emitterPin = A1;
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

// char
#define charQuantity 43
char charSet[charQuantity] = {
  ' ', '-', '.', '2', '3', '4', '5', '6', '7', '8',
  '9', '*', 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H',
  'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R',
  'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', '+', '+',
  '+', '+', '+'
};
String labelString;
int charHome = 21;
bool waitingLabel = false;

// infrared
int minLight;
int maxLight;
int thresholdLight;
int currentCharPosition = -1;
int deltaPosition;
bool reverseCalibration;

void setup() {
	Serial.begin(9600);
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

	setHome(true);
}

void setHome(bool calibrate) {
	digitalWrite(emitterPin, HIGH);
	if (calibrate) {
		stepperChar.setCurrentPosition(0);
		minLight = analogRead(sensorPin);
		maxLight = analogRead(sensorPin);
		Serial.println("calibrating...");

		stepperChar.moveTo(stepsPerRevolution * 1.1f);
		while (stepperChar.distanceToGo() > 0) {
			sensorState = analogRead(sensorPin);
			int averaged = sensorStateAvg.reading(sensorState);

			if (minLight != 0 && averaged < minLight) {
				minLight = averaged;
			}
			if (maxLight != 1023 && averaged > maxLight) {
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

	if (reverseCalibration) {
		targetPosition *= -1;
	}

	stepperChar.moveTo(targetPosition);
	sensorState = analogRead(sensorPin);

	//Serial.print("sensor ");
	//Serial.println(sensorState);
	sensorStateAvg.reset();
	int averaged = sensorStateAvg.reading(sensorState);

	while (averaged < thresholdLight) {
		sensorState = analogRead(sensorPin);
		averaged = sensorStateAvg.reading(sensorState);
		stepperChar.run();
		//if (sensorState > thresholdLight) Serial.println(sensorState);
	}
	stepperChar.setCurrentPosition(0);

	//Serial.print("last reading: ");
	//Serial.println(sensorState);
	currentCharPosition = charHome;
	if (reverseCalibration) {
		//Serial.println("<< reverse");
		stepperChar.runToNewPosition(-stepsPerChar);
		stepperChar.setCurrentPosition(0);
		reverseCalibration = false;
	}
	else {
		//Serial.println(">> forward");
	}

	//Serial.print("at home, position ");
	//Serial.println(currentCharPosition);

	digitalWrite(emitterPin, LOW);
	delay(500);
}

void feedLabel() {
	stepperFeed.runToNewPosition(stepperChar.currentPosition() - stepsPerRevolution / 2);
	delay(200);
}

void pressLabel() {
	for (int pos = restAngle; pos < peakAngle; pos++) {
		myServo.write(pos);
		delay(12);
	}
	delay(500);
	for (int pos = peakAngle; pos > restAngle; pos--) {
		myServo.write(pos);
		delay(12);
	}
}

void writeLabel(String label) {
	feedLabel();
	for (int i = 0; i < label.length(); i++) {
		goToCharacter(label[i]);
		
		setHome(false);
	}
	goToCharacter('*');

	Serial.println("finished");
	setHome(false);
}

void goToCharacter(char c) {

	int backAdditional = 0;

	if (c == '0') {
		//Serial.println("exception ZERO");
		c = 'O';
	}
	else if (c == '1') {
		//Serial.println("exception ONE");
		c = 'I';
	}

	for (int i = 0; i < charQuantity; i++) {
		if (c == charSet[i]) {
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
	feedLabel();
}

void readSerial() {
	if (!waitingLabel) {
		Serial.println("write label and hit enter");
		waitingLabel = true;
	}
	while (Serial.available()) {
		labelString = Serial.readStringUntil('\n');
		Serial.print("TAG: ");
		Serial.println(labelString);

		writeLabel(labelString);
		waitingLabel = false;
	}
}

void loop() {
	readSerial();
}