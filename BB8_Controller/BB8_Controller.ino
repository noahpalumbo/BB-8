/*
* BB-8 Controller Transceiver Module
* RF chip used: nRF24L01
* Updated: Feb 2020 by Noah Palumbo
*/

#include "controllerLib.h"

/****************** RF Config ***************************/
// Create "radio" object from CE and CSN pins */
// PIN  6 =  CE : Chip Enable (data transmission logic)
// PIN 10 = CSN : Chip Select Not (SPI enable pin, more transmission logic)
RF24 radio(6, 10);  // can be any digital pins, we use 6 and 10
/**********************************************************/

// addresses of each chip
byte addresses[][6] = {"1Node","2Node"};
//                      Tx:[0], Rx[1]

// declare motor and servo ints
int x1, x2, y1, y2, cw, ccw;

void setup() {          // Setup only runs once!
  Serial.begin(9600); // begin communication with serial monitor @ specified baud 
  
  radio.begin();
  
  // sets pin modes for each peripheral
  setPins();
  
  // sets power amplifier level, set low for testing because chips likely to be close together
  // can set high for longer distances, but draws more current
  radio.setPALevel(RF24_PA_LOW);
  //radio.setChannel(108);                // only used if want to subscribe to specific RF channel
  //Serial.println(radio.getChannel());   // echoes RF channel

  // Used to control whether this node is sending or receiving
  // Open a writing and reading pipe on each radio, with opposite addresses
  // This transceiver will only be transmitting for the time being...
  radio.openWritingPipe(addresses[0]);
  // but we may read from Radio 1 in the future
  radio.openReadingPipe(1,addresses[1]);
    
  radio.stopListening(); // First, stop listening so we can talk.
}

// this code typically runs over and over
void loop() {
  uint8_t message[5];        // initiate message at 0
  x2 = analogRead(A0);            // X2 : X motors, Analog value 0-1023
  x2 = map(x2, 0, 1023, 0, 255);  // map instead to 0-255 so X2 message is 8-bit rather than 10-bit
  y2 = analogRead(A1);            // Y2 : Y motors, Analog value 0-1023
  y2 = map(y2, 0, 1023, 0, 255);  // map instead to 0-255 so Y2 message is 8-bit rather than 10-bit
  x1 = analogRead(A7);            // X1 : X neck servos, Analog value 0-1023
  x1 = map(x1, 0, 1023, 0, 255);  // map instead to 0-255 so Y2 message is 8-bit rather than 10-bit
  y1 = analogRead(A6);            // Y1 : Y neck servos, Analog value 0-1023
  y1 = map(y1, 0, 1023, 0, 255);  // map instead to 0-255 so Y2 message is 8-bit rather than 10-bit
  
  cw = digitalRead(A5);           // 0 = pressed, 1 = not pressed
  ccw = digitalRead(A3);          // 0 = pressed, 1 = not pressed

  message[0] = x2;                      // X2 (X motors) will be first byte     [X2]
  message[1] = y2;                      // Y2 (Y motors) will be second byte    [X2, Y2]
  message[2] = x1;                      // X1 (X servo)  will be third byte     [X2, Y2, X1]
  message[3] = y1;                      // Y1 (Y servo)  will be fourth byte    [X2, Y2, X1, Y1]
  message[4] = cw;                      // CW (Clockwise Bit)                   [X2, Y2, X1, Y1, 0000 00(cw)]
  message[4] = (message[4] << 1) | ccw; // CCW (CounterClockwise Bit)           [X2, Y2, X1, Y1, 0000 00(cw)(ccw)]

  // Serial Message Printing
  printMessage(message);
  
  // Compund instruction, but most important
  // radio.write() returns a boolean 0 or 1
  // 0 return means the write was unsuccessful, meaning it was not received by receiver
  // 1 return means write was successful
  // CALLS radio.write() and checks its value
  if (!radio.write(&message, sizeof(message) )){      // if 0 returned 
   Serial.println(F("failed"));                         //    print "failed"
  }

  // Short delay before next payload sent
  //delay(100);
} // Loop back
