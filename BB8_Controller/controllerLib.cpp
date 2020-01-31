#include <Arduino.h>

void setPinModes()
{
    // Set pinModes for each pin in use
  // Outputs
  pinMode(6, OUTPUT); // OUTPUT to CE of RF chip
  // Inputs all from joystick pins
  pinMode(A0, INPUT); // X2 wheels x
  pinMode(A1, INPUT); // Y2 wheels y
  pinMode(A3, INPUT_PULLUP);
  pinMode(A5, INPUT_PULLUP);
  pinMode(A6, INPUT); // Y1 neck y
  pinMode(A7, INPUT); // X1 neck x
 }
