/*
  BB-8 Head Transceiver Module
  RF chip used: nRF24L01
  Updated: Mar 2020 by Noah Palumbo
*/

// include libraries that make our lives easier
#include <SPI.h>   // SPI is protocol Arduino uses to communicate with RF chip
#include "RF24.h"  // contains functions that help us use the RF chip
#include <Arduino.h>

/****************** User Config ***************************/
// assign GPIO pin #s for each audio function
#define audio_1 4
#define audio_2 5
#define audio_3 6
#define audio_4 7

int audioIn;    // bits in payload[4], values can range from 0-3
int prevIn = 3;

// Disconnect Mode Timer
int disconnect_counter = 0;
const int disconnect_threshold = 5;
/****************** RF Config ***************************/
// Create "radio" object from CE and CSN pins */
// PIN  9 =  CE : Chip Enable (data transmission logic)
// PIN 10 = CSN : Chip Select Not (SPI enable pin, more transmission logic)
RF24 radio(9, 10);

// addresses of each chip
byte addresses[][6] = {"1Node","2Node,3Node"};
//                      Tx:[0],Rx:[1],Rx:[2]
/**********************************************************/

void setup()
{
  Serial.begin(9600);

  radio.begin();

  // Set the PA Level low to prevent power supply related issues and 
  // bc of the likelihood of close proximity of the devices. RF24_PA_MAX is default.
  radio.setPALevel(RF24_PA_LOW);

  // Open reading pipe #1, listening for "1Node" (controller)
  radio.openReadingPipe(1, addresses[0]);
  
  // Start the radio listening for data
  radio.startListening();
  
  /* Pins for GPIO control over BB8_Audio */
  pinMode(audio_1, OUTPUT);         // set the digital pin 8 as output
  pinMode(audio_2, OUTPUT);         // set the digital pin 9 as output
  pinMode(audio_3, OUTPUT);         // set the digital pin 10 as output
  pinMode(audio_4, OUTPUT);         // set the digital pin 11 as output

  /* Make sure pins aren't floating at startup */
  digitalWrite(audio_1, LOW);
  digitalWrite(audio_2, LOW);
  digitalWrite(audio_3, LOW);
  digitalWrite(audio_4, LOW);
}

void loop()
{
  uint8_t payload[5]; // payload variable 5 bytes

  if (radio.available()) {                                  // if controller is connected
    Serial.println("Radio received!");
    radio.read( &payload, sizeof(uint8_t)*5 );             // Get the payload

  // Reset Disconnect Counter since message was received
  disconnect_counter = 0;
  
  // FOR USE IF CONTROLLER NEEDS FEEDBACK
  //radio.stopListening();                                        // First, stop listening so we can talk
  //radio.write( &payload, sizeof(unsigned long) );               // Send the final one back.
  //radio.startListening();                                       // Now, resume listening so we catch the next packets.

  // Decode our message into individual motor and servo signals
  // Payload structure:
  //  [ ---- ---- , ---- ---- , ---- ---- , ---- ---- , 00--    00-- ]
  //     Motor X     Motor Y      Neck X      Neck Y   Audio  Head Rotation
  //-------------------------------------------------------

  }// radio.available()
  
  else{  // controller not connected
        Serial.print("Disconnect Counter = ");
        Serial.println(disconnect_counter);
        if(disconnect_counter < disconnect_threshold)
          disconnect_counter++;
        else
        {
          // Alert serial monitor
          Serial.println("CONTROLLER NOT CONNECTED");
          // TODO Give inputs 0 values
        }
     }
  
  // Debug: get serial input for control
  // char c = Serial.read();

  audioIn = (payload[4] & 0x30) >> 4;

  /* Only want to play one sound per "press" of button */
  if (!((prevIn == 1) /*|| (prevIn == 2)*/)) // if a button wasn't already pressed
  {
    if (audioIn == 1)                   // IF BUTTON 1 PRESSED
    {
      Serial.println("Audio_1 raised...");
      digitalWrite(audio_1, HIGH);    // raise gpio for 100ms
      delay(100);
      digitalWrite(audio_1, LOW);
      Serial.println("Audio_1 lowered.");
    }
    /*if (payload[4] == 2)                   // IF BUTTON 2 PRESSED (NOTE CURRENTLY BUTTON 2 IS NOTE PHYSICALLY OR VIRTUALLY CONNECTED)
    {                                        // I will leave the code commented for future reference if you want to add another audio interface (like a button)
      Serial.println("Audio_2 raised...");
      digitalWrite(audio_2, HIGH);    // raise gpio for 100ms
      delay(100);
      digitalWrite(audio_2, LOW);
      Serial.println("Audio_2 lowered...");
    }*/
  }
  prevIn = audioIn;  // update previous input
  
  delay(200);
} // Loop
