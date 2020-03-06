#include "controllerLib.h"

void setPins()
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
  Serial.println("Pins set");
}

void printMessage(uint8_t message[])
{
  int i = 0;
  
  /******** Can check Serial Monitor to see what is being Sent ***********/
  Serial.print("Transmitted: ");
  while(i < 4)
  {
    Serial.print(message[i], BIN);
    i++;
  }
  Serial.println(message[4] , BIN);
  i = 0;

  Serial.print("Motor X-Value:");
  Serial.println(message[0]);
    
  Serial.print("Motor Y-Value:");
  Serial.println(message[1]);

  Serial.print("Neck X-Value:");
  Serial.println(message[2]);

  Serial.print("Neck Y-Value:");
  Serial.println(message[3]);

  Serial.print("Right Stick Click:");       // 0 = pressed, 1 = not pressed
  Serial.println((message[4] & 0x02) >> 1);

  Serial.print("Left Stick Click:");        // 0 = pressed, 1 = not pressed
  Serial.println(message[4] & 0x01);
  
  Serial.println();
  
  /***********************************************************************/
}
