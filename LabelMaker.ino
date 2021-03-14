#include <Stepper.h>
//#include <AccelStepper.h>
#include <Servo.h>

#define HALFSTEP 8

#define motorPin1  8     // IN1 on ULN2003 ==> Blue   on 28BYJ-48
#define motorPin2  9     // IN2 on ULN2004 ==> Pink   on 28BYJ-48
#define motorPin3  10    // IN3 on ULN2003 ==> Yellow on 28BYJ-48
#define motorPin4  11    // IN4 on ULN2003 ==> Orange on 28BYJ-48

// home sensor
int sensorPin = A0;
int emitterPin = A1;
int sensorState;

// servo and steppers
const int stepsPerRevolution = 2048;
Stepper stepperFeed(stepsPerRevolution, 2, 4, 3, 5);
Stepper stepperChar(stepsPerRevolution, 8, 10, 9, 11);
//AccelStepper stepperFeed(HALFSTEP, 2, 4, 3, 5);
//AccelStepper stepperChar(HALFSTEP, 8, 10, 9, 11);
//int endPoint = 4076;


Servo myServo;
int restAngle = 20;
int peakAngle = 64;
int stepsPerChar;

// chars
#define charQuantity 43
char charSet[charQuantity] = {
  ' ', '-', '.', '2', '3', '4', '5', '6', '7', '8',
  '9', '*', 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H',
  'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R',
  'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', '+', '+',
  '+', '+', '+'
};
String labelString;
bool waitingLabel = false;

// homing
int minLight;
int maxLight;
int thresholdLight;
int currentCharPosition = -1;
int deltaPosition;

void setup() {
	Serial.begin(9600);
	pinMode(sensorPin, INPUT);
	pinMode(emitterPin, OUTPUT);


	/*
	stepperFeed.setMaxSpeed(1000.0);
	stepperFeed.setAcceleration(100.0);
	stepperFeed.setSpeed(200);

	stepperChar.setMaxSpeed(1000.0);
	stepperChar.setAcceleration(1000.0);
	stepperChar.setSpeed(50);
	*/
	stepperFeed.setSpeed(16);
	stepperChar.setSpeed(16);



	myServo.attach(12);
	myServo.write(restAngle);
	delay(100);

	stepsPerChar = stepsPerRevolution / charQuantity;

	setHome(true);
	
}

void test() {
	//for (int i = 0; i < 30 * charQuantity; i++) {
		//delay(1000);
		//stepperChar.step(stepsPerRevolution/charQuantity);
	/*
	if (stepperChar.distanceToGo() == 0){
		stepperChar.setCurrentPosition(0);
		endPoint = -endPoint;

		stepperChar.moveTo(endPoint);
	}

		stepperChar.run();*/
	//}
}

void setHome(bool calibrate) {
	digitalWrite(emitterPin, HIGH);

	if (calibrate) {
		minLight = analogRead(sensorPin);
		maxLight = analogRead(sensorPin);
		Serial.println("calibrating...");
		for (int i = 0; i < stepsPerRevolution; i++) {
			stepperChar.step(1);
			sensorState = analogRead(sensorPin);

			if (minLight != 0 && sensorState < minLight) {
				minLight = sensorState;
			}
			if (maxLight != 1023 && sensorState > maxLight) {
				maxLight = sensorState;
			}
			thresholdLight = (minLight + maxLight) / 2;
		}
	}

	Serial.print("calling home...");
	/*
	Serial.print(", reading :");
	Serial.println(sensorState);
	if (sensorState > thresholdLight) Serial.print("X");
	Serial.print(" / threshold :");
	Serial.print(thresholdLight);
	Serial.print(" / minLight :");
	Serial.print(minLight);
	Serial.print(" / maxLight: ");
	Serial.println(maxLight);
	*/

	delay(500);

	for (int i = 0; i < stepsPerRevolution; i++) {
		sensorState = analogRead(sensorPin);

		/*
		Serial.print("sample ");
		Serial.print(i);
		*/

		//bool backwards;

		if (sensorState < thresholdLight) {
			//if (currentCharPosition == -1 || currentCharPosition < 22) {
			stepperChar.step(1);
			//	backwards = false;
			//} else if (currentCharPosition > 22) {
			//	stepperChar.step(-1);
			//	backwards = true;
			//}
		} else {
			//if (backwards) {
			//	stepperChar.step(-stepsPerChar);
			//}
			currentCharPosition = 22;
			Serial.print("at home, position ");
			Serial.println(currentCharPosition);
			digitalWrite(emitterPin, LOW);
			break;
		}
	}
	delay(500);
}

void feedLabel() {
	for (int i = 0; i < 50; i++) {
		stepperFeed.step(-stepsPerRevolution / 7);
	}
	delay(200);
}

void pressLabel() {
	for (int pos = restAngle; pos < peakAngle; pos++) {
		myServo.write(pos);
	}
	delay(1000);
	for (int pos = peakAngle; pos > restAngle; pos--) {
		myServo.write(pos);
	}
	delay(200);
}

void writeLabel(String label) {

	//feedLabel();

	for (int i = 0; i < label.length(); i++) {
		goToCharacter(label[i]);
		setHome(false);
	}
	goToCharacter('*');

	Serial.println("finished");
	setHome(false);
}

void goToCharacter(char c) {
	for (int i = 0; i < charQuantity; i++) {
		if (c == charSet[i]) {
			deltaPosition = i - currentCharPosition + 1;
			/*
			if (deltaPosition < 0)
				deltaPosition += charQuantity;
			else if (deltaPosition > 0)
				deltaPosition -= charQuantity;
			*/

			Serial.print("char ");
			Serial.print(c);
			Serial.print(" is on position ");
			Serial.print(i);
			Serial.print(" ( delta ");
			Serial.print(deltaPosition);
			Serial.println(")");
			currentCharPosition = i;


		}
	}
	stepperChar.step(stepsPerChar * deltaPosition);
	delay(200);
	pressLabel();
	//feedLabel();
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
	//test();
	
}
