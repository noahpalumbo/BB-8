
/*
* Getting Started example sketch for nRF24L01+ radios
* This is a very basic example of how to send data from one node to another
* Updated: Dec 2014 by TMRh20
*/

#include <SPI.h>
#include "RF24.h"

/****************** User Config ***************************/
/***      Set this radio as radio number 0 or 1         ***/
bool radioNumber = 0;

/* Hardware configuration: Set up nRF24L01 radio on SPI bus plus pins 7 & 8 */
RF24 radio(6, 5);
/**********************************************************/

byte addresses[][6] = {"1Node","2Node"};

// Used to control whether this node is sending or receiving
bool role = 0;

void setup() {
  Serial.begin(115200);
  Serial.println(F("RF24/examples/GettingStarted"));
  Serial.println(F("*** PRESS 'T' to begin transmitting to the other node"));

  radio.begin();
  pinMode(2, OUTPUT);
  pinMode(3, OUTPUT);
  pinMode(4, OUTPUT);
  pinMode(5, OUTPUT);
  pinMode(6, OUTPUT);
  pinMode(7, OUTPUT);
  pinMode(A6, INPUT);
  pinMode(A7, INPUT);

  // Set the PA Level low to prevent power supply related issues since this is a
 // getting_started sketch, and the likelihood of close proximity of the devices. RF24_PA_MAX is default.
  radio.setPALevel(RF24_PA_LOW);
  radio.setChannel(108);
  Serial.println(radio.getChannel());
  
  // Open a writing and reading pipe on each radio, with opposite addresses
  if(radioNumber){
    radio.openWritingPipe(addresses[1]);
    radio.openReadingPipe(1,addresses[0]);
  }else{
    radio.openWritingPipe(addresses[0]);
    radio.openReadingPipe(1,addresses[1]);
  }
  
  // Start the radio listening for data
  radio.startListening();
}

void loop() {
  unsigned int data;
  int x, y;
  
  if(radio.available()){                                                                // Variable for the received timestamp
    /*while (radio.available()) {                                   // While there is data ready
      radio.read( &data, sizeof(unsigned int) );             // Get the payload
    }
    
    */
    //y = data & 0xFF;
    //x = (data >> 8) & 0xFF;
    x = analogRead(A7);
    y = analogRead(A6);
    //leftMotorSpeed += (y - 127) + x;
    //rightMotorSpeed += (y - 127) - x;

    //leftMotorSpeed *= 2;
    //rightMotorSpeed *= 2;
    
    //leftMotorSpeed = constrain(leftMotorSpeed, -255, 255);
    //rightMotorSpeed = constrain(rightMotorSpeed, -255, 255);
 
    x = constrain((x-512)*2, -1023, 1023);
    y = constrain((y-512)*2, -1023, 1023);
    analogWrite(7, abs(x));
    analogWrite(2, abs(y));
    x = map(x, -513, 513, -255, 255);
    y = map(y, -513, 513, -255, 255);
   
    Serial.print(x);
    Serial.print(" ");
    Serial.println(y);
    if(y < -10){
       digitalWrite(3, HIGH);
       digitalWrite(4, LOW);
    }
    else if(y > 10){
       digitalWrite(3, LOW);
       digitalWrite(4, HIGH);
    }
    else{
      digitalWrite(3, LOW);
      digitalWrite(4, LOW);
    }
  
    if(x < -10){
      digitalWrite(5, HIGH);
      digitalWrite(6, LOW);
    }
    else if(x > 10){
       digitalWrite(5, LOW);
       digitalWrite(6, HIGH);
    }
    else{
      digitalWrite(5, LOW);
      digitalWrite(6, LOW);
    }
  }
} // Loop

