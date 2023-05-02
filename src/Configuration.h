#pragma once

#include <Arduino.h>

/**
 * Basic Configuration
 *
 * Depending on the hardware you've used to build your E-TKT, you might need to
 * enable some of these constants to get the hardware into a working state
 */

/**
 * If your hall sensor has inverted logic (eg active LOW and neutral HIGH)
 * uncomment this #define to invert the logic checking it. If you're affected by
 * this then you'll see the character carousel move forward slightly and then
 * stop when the E-TKT starts up instead of moving to the "J" position.
 *
 * For instance, invert if using a "3144" hall sensor but don't invert if using
 * a "44E 402" hall sensor. If you're not sure if you need this, try it both
 * ways and see which one works.
 */
#define INVERT_HALL_SENSOR_LOGIC false

/**
 * If your feed motor moves in the wrong direction by default, uncomment the
 * define below to reverse it. It should be obvious if this is happening since
 * the tape gets fed in the wrong direction.
 */
#define REVERSE_FEED_STEPPER_DIRECTION false

/**
 * Speed and acceleration of the stepper motor that feeds the label tape,
 * measured in steps/s and steps/s^2. Use lower values if you find that the
 * printer doesn't consistently feed the correct length of tape between letters.
 * For calibrating these values the same advice about the character stepper
 * motor above applies. 4076 steps is one full revolution of the motor.
 */
#define FEED_STEPPER_MAX_SPEED 1000000
#define FEED_STEPPER_MAX_ACCELERATION 1000

/**
 * Speed and acceleration of the stepper motor that rotates the character
 * carousel, measured in steps/s and steps/s^2. Use lower values if you find
 * that the printer sometimes prints the wrong letter.  Any value above zero is
 * ok but lower values will slow down printing, if you're having trouble start
 * by halving them and move up from there.  The speed you can reliably achieve
 * depends on the quality of the motor, how much current you've set it up to
 * use, and how fast the ESP-32 can talk with it. 1600 steps is a full
 * revolution of the carousel.
 */
#define CHARACTER_STEPPER_MAX_SPEED 320000
#define CHARACTER_STEPPER_MAX_ACCELERATION 16000

/**
 * Debugging
 *
 * Alter these to enable/disable individual parts of the hardware while
 * troubleshooting.
 */
#define ENABLE_SOUND true
#define ENABLE_FEED true
#define ENABLE_CUT true
#define ENABLE_PRESS true
#define ENABLE_CAROUSEL true

/**
 * Development
 *
 * Developers of the firmware may find it useful to turn on these features
 */
#define ENABLE_SERIAL false  // Enables serial output
#define ENABLE_OTA false     // Enables OTA updates at http://e-tkt.local/update
#define DEBUG_WIFI false     // Enables WiFi debugging

/**
 * Hardware Pins
 */
#define HALL_PIN 34        // hall sensor
#define WIFI_RESET_PIN 13  // tact switch
#define SERVO_PIN 14       // Press servo
#define CHARACTER_LED_PIN 17
#define FINISH_LED_PIN 5
#define PIN_STEPPER_CHAR_STEP 32
#define PIN_STEPPER_CHAR_DIR 33
#define PIN_STEPPER_CHAR_ENABLE 25
#define BUZZER_PIN 26

/**
 * Physical Characteristics
 *
 * Physical characteristics of the printer.  These are used to calculate the
 * number of steps required to move and are unlikely to ever need to be changed.
 */

#define CHAR_MICROSTEPS 16
#define CHAR_STEP_COUNT 200
// depending on the hall sensor positioning, the variable below makes sure the
// initial calibration is within tolerance use a value between -1.0 and 1.0 to
// make it roughly align during assembly
#define ASSEMBLY_CALIBRATION_ALIGN 0.5f
#define CHAR_HOME_POSITION 21

// depending on servo characteristics and P_press assembling process, the
// pressing angle might not be so precise and the value below compensates it use
// a value between 0 and 20 to make sure the press is barely touching the daisy
// wheel on test align
#define ASSEMBLY_CALIBRATION_FORCE 15
#define MICROSTEPS_FEED 8
#define FEED_MOTOR_STEPS_PER_REVOLUTION 4076
#define HALL_SENSOR_THRESHOLD 128  // between 0 and 4096
