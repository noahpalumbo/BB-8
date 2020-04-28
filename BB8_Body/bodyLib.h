#ifndef BODY_H
#define BODY_H

// include libraries that make our lives easier
#include <SPI.h>   // SPI is protocol Arduino uses to communicate with RF chip
#include "RF24.h"  // contains functions that help us use the RF chip
#include <Servo.h> // contains functions to operate servos
#include <Arduino.h>

/************ Pin Number Assignment **************************/
// Pins used to control X and Y axis on motors
const int X_PWM_PIN = 2;
const int X_DIR1_PIN = 3;
const int Y_PWM_PIN = 7;
const int Y_DIR2_PIN = 5;

const int X_PWM_PIN2 = 9;
const int X_DIR1_PIN2 = 33;
const int Y_PWM_PIN2 = 10;
const int Y_DIR2_PIN2 = 34;

// connect motor controller pins to Arduino digital pins
// Neck Spinning Pins
const int rotEn = 25;
const int rotDir1 = 24;
const int rotDir2 = 23;

// Pins for Servos
const int s1pin = 11;
const int s2pin = 12;
/*************************************************************/

// Set damping constant
// if need more intense damping, use a map system like the servos
const int dampingConstant = 1;
const int fastDampingConstant = 5 * dampingConstant;  // use when need a quicker damp (mainly for deceleration [yeah I know it's just acceleration in a different direction])

// Set Movement Motor Zero Bounds
// set values that motor values need to pass in order to send signal to motor
const int motorPosBound = 20;
const int motorNegBound = -20;

// sets pin modes for each peripheral
void setPins();

// serial payload debug printing
void printPayload(uint8_t payload[]);

// motor control and damping
int negativeInputDamp(int input, int curr);
int positiveInputDamp(int input, int curr);
int zeroInputDamp(int input, int curr);

#endif // BODY_H
