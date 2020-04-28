/*
* BB-8 Body Transceiver Module
* RF chip used: nRF24L01
* Updated: Mar 2020 by Noah Palumbo
*/

#include "bodyLib.h"

/****************** User Config ***************************/
// Create servo objects
Servo SERVO1;
Servo SERVO2;

// Set initial motor values
int currMotorY = 0;
int currMotorX = 0;

// Disconnect Mode Timer
int disconnect_counter = 0;
const int disconnect_threshold = 5;
/**********************************************************/

/****************** RF Config ***************************/
// Create "radio" object from CE and CSN pins */
// PIN  8 =  CE : Chip Enable (data transmission logic)
// PIN 53 = CSN : Chip Select Not (SPI enable pin, more transmission logic)
RF24 radio(8, 53);

// addresses of each chip
byte addresses[][6] = {"1Node","2Node,3Node"};
//                      Tx:[0],Rx:[1],Rx:[2]
/**********************************************************/

void setup() {
  Serial.begin(9600);

  radio.begin();

  // Set the PA Level low to prevent power supply related issues and 
  // bc of the likelihood of close proximity of the devices. RF24_PA_MAX is default.
  radio.setPALevel(RF24_PA_LOW);

  // Open reading pipe #1, listening for "1Node" (controller)
  radio.openReadingPipe(1, addresses[0]);

  // Start the radio listening for data
  radio.startListening();

  //initialize servos and start them in the center
  SERVO1.attach(s1pin);
  SERVO2.attach(s2pin);
  SERVO1.write(81);     // 
  SERVO2.write(46);     // may need tuning as we make changes
}

void loop() {
  uint8_t payload[5]; // payload variable 5 bytes

  int motorY;     //Joystick 1 Y-axis
  int motorX;     //Joystick 1 X-axis
  int neckX;      // Joystick 2 X-axis
  int neckY;      // Joystick 2 Y-axis
  int rotate;     // Head rotation

  //servo variables
  int servo1angle;
  int servo2angle;
  
  if (radio.available()) {                                  // if controller is connected
    radio.read( &payload, sizeof(uint8_t)*5 );             // Get the payload

    // Reset Disconnect Counter since message was received
    disconnect_counter = 0;
    
    // FOR USE IF CONTROLLER NEEDS FEEDBACK
    //radio.stopListening();                                        // First, stop listening so we can talk
    //radio.write( &payload, sizeof(unsigned long) );               // Send the final one back.
    //radio.startListening();                                       // Now, resume listening so we catch the next packets.

    //Decode our message into individual motor and servo signals
    // Payload structure:
    //  [ ---- ---- , ---- ---- , ---- ---- , ---- ---- , 00--     00-- ]
    //     Motor X     Motor Y      Neck X      Neck Y    Audio   Head Rotation
    //-------------------------------------------------------

    /* Parse Payload into array fragments (bytes) */
    motorX = payload[0];
    motorY = payload[1];
    neckX = payload[2];
    neckY = payload[3];
    rotate = payload[4];    // 0 & 3 do nothing, 1 and 2 rotate in respective directions

    
    // Print Payload Fragments
    //printPayload(payload);
  }

  else{  // controller not connected
        Serial.print("Disconnect Counter = ");
        Serial.println(disconnect_counter);
        if(disconnect_counter < disconnect_threshold)
          disconnect_counter++;
        else
        {
          // Alert serial monitor
          Serial.println("CONTROLLER NOT CONNECTED");
          // Give inputs 0 values
          motorX = 127; // Joystick range = [0,255]
          motorY = 127; // so center @ ~127
          neckX = 127;
          neckY = 127;
          rotate = 3;
        }
     }

    // Joystick values can range -127 and +127, set from 127 to 0 by subtracting 127
    motorY -= 127;
    motorX -= 127;
    neckY -= 127;
    neckX -= 40;

    // Limit Motor Values to +/- 128
    if(motorX < -127)     // Otherwise the motor value
      motorX = -127;      // can overflow and the value
    else if(motorX > 127) // resets straight to 0
      motorX = 127;       // Very bad for the motors
    if(motorY < -127)
      motorY = -127;
    else if(motorY > 127)
      motorY = 127;

    // Modified to Zero Center Payload Debug Prints
    // values roughly (-127 <-> 127)
    /*Serial.print("motorX: ");
    Serial.println(motorX);
    Serial.print("motorY: ");
    Serial.println(motorY);
    Serial.print("neckX: ");
    Serial.println(neckX);
    Serial.print("neckY: ");
    Serial.println(neckY);
    Serial.print("rotate: ");
    Serial.println(rotate);
    Serial.println();*/
    

    // Motor Damping Debug Prints
    Serial.print("motorY: ");
    Serial.println(motorY);
    Serial.print("currMotorY: ");
    Serial.println(currMotorY);
    
    Serial.print("motorX: ");
    Serial.println(motorX);
    Serial.print("currMotorX: ");
    Serial.println(currMotorX);
    
    Serial.println();

    
    // scaling to account for overflow
    neckX = neckX / 2;
    neckY = neckY / 1.5;
    
    //SERVO OUTPUT CODE
    servo1angle = neckY;
    servo2angle = neckX;
    servo1angle = map(servo1angle, 127, -127, 37, 128);
    servo2angle = map(servo2angle, -40, 22, 0, 92);
    SERVO1.write(servo1angle); // write neckY
    SERVO2.write(servo2angle); // write neckX
    /* If movement too janky may need to use damping code similar to motors */

    // MOTOR OUTPUT CODE
        // Motor Y Direction and currMotorY update
        if (motorY < motorNegBound)                    // if user input is negative Y
          currMotorY = negativeInputDamp(motorY, currMotorY);
        else if (motorY > motorPosBound)                // if user input is positive Y
          currMotorY = positiveInputDamp(motorY, currMotorY);
        else     // user input is zero
          currMotorY = zeroInputDamp(motorY, currMotorY);


        // Send currMotorY to correct pins
        if(currMotorY < 0)
        {         
          digitalWrite(Y_DIR2_PIN, 0);
          digitalWrite(Y_DIR2_PIN2, 1);
          analogWrite(Y_PWM_PIN, abs(currMotorY) * 2);
          analogWrite(Y_PWM_PIN2, abs(currMotorY) * 2);
        }
        else if (currMotorY > 0)
        {
          digitalWrite(Y_DIR2_PIN, 1);
          digitalWrite(Y_DIR2_PIN2, 0);
          analogWrite(Y_PWM_PIN, abs(currMotorY) * 2);
          analogWrite(Y_PWM_PIN2, abs(currMotorY) * 2);
        }
        else
        {
           digitalWrite(Y_DIR2_PIN, 0);
           analogWrite(Y_PWM_PIN, 0);
           digitalWrite(Y_DIR2_PIN2, 0);
           analogWrite(Y_PWM_PIN2, 0);
        }     
        

        // Motor X Direction and currMotorX update
        if (motorX < motorNegBound)                     // if user input is negative X
          currMotorX = negativeInputDamp(motorX, currMotorX);
        else if (motorX > motorPosBound)                 // if user input is positive X
          currMotorX = positiveInputDamp(motorX, currMotorX);
        else                                 // user input is zero
          currMotorX = zeroInputDamp(motorX, currMotorX);
        
        // Send currMotorX to correct pins
        if(currMotorX < 0)
        {         
          digitalWrite(X_DIR1_PIN, 0);
          digitalWrite(X_DIR1_PIN2, 1);
          analogWrite(X_PWM_PIN, abs(currMotorX) * 2);
          analogWrite(X_PWM_PIN2, abs(currMotorX) * 2);
        }
        else if (currMotorX > 0)
        {
          digitalWrite(X_DIR1_PIN, 1);
          digitalWrite(X_DIR1_PIN2, 0);
          analogWrite(X_PWM_PIN, abs(currMotorX) * 2);
          analogWrite(X_PWM_PIN2, abs(currMotorX) * 2);
        }
        else
        {
           digitalWrite(X_DIR1_PIN, 0);
           analogWrite(X_PWM_PIN, 0);
           digitalWrite(X_DIR1_PIN2, 0);
           analogWrite(X_PWM_PIN2, 0);
        }

                 
        /*Serial.println("BEFORE");
        Serial.print("Rotate: ");
        Serial.println(rotate);
        Serial.print("rotEn: ");
        Serial.println(rotEn);
        Serial.print("rotDir1: ");
        Serial.println(rotDir1);
        Serial.print("rotDir2: ");
        Serial.println(rotDir2);
        Serial.println();*/

        //Serial.println(rotate);
        if (rotate == 1)
        {
            digitalWrite(rotDir1, HIGH);
            digitalWrite(rotDir2, LOW);
            // set speed to 50 out of possible range 0~255
            analogWrite(rotEn, 150);
        }
        else if (rotate == 2)
        {
            digitalWrite(rotDir1, LOW);
            digitalWrite(rotDir2, HIGH);
            analogWrite(rotEn, 150);
        }
        else 
        {
            digitalWrite(rotDir1, LOW);
            digitalWrite(rotDir2, LOW);
        }

        /*Serial.println("AFTER");
        Serial.print("rotEn: ");
        Serial.println(rotEn);
        Serial.print("rotDir1: ");
        Serial.println(rotDir1);
        Serial.print("rotDir2: ");
        Serial.println(rotDir2);
        Serial.println();*/

       // Delay to slow everything down for testing
       delay(200);
} // Loop

//Motor 1 is janky
