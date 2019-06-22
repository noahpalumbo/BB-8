/*
  Getting Started example sketch for nRF24L01+ radios
  This is a very basic example of how to send data from one node to another
  Updated: Dec 2014 by TMRh20
*/

#include <SPI.h>
#include "RF24.h"
#include <Servo.h>

/****************** User Config ***************************/
/***      Set this radio as radio number 0 or 1         ***/
bool radioNumber = 1;

// Pins used to control X and Y axis on motors
const int X_PWM_PIN = 2;  //wheel 1
const int X_DIR1_PIN = 3;
const int Y_PWM_PIN = 7;  //wheel 2
const int Y_DIR2_PIN = 5;

const int X_PWM_PIN2 = 9; //wheel 3
const int X_DIR1_PIN2 = 33;
const int Y_PWM_PIN2 = 10;  //wheel 4
const int Y_DIR2_PIN2 = 34;

// Pins for servos
Servo SERVO1;
Servo SERVO2;
const int s1pin = 11;
const int s2pin = 12;

// Set initial motor values
int currMotorY = 0;
int currMotorX = 0;

// Set damping constant
// if need more intense damping, use a map system like the servos
int dampingConstant = 1;
int fastDampingConstant = 5 * dampingConstant;  // use when need a quicker damp (mainly for deceleration [yeah I know it's just acceleration in a different direction])

/* Hardware configuration: Set up nRF24L01 radio on SPI bus plus pins 7 & 8 */
RF24 radio(8, 53);
/**********************************************************/

byte addresses[][6] = {"1Node", "2Node"};

// Used to control whether this node is sending or receiving
//NOTE we will always be reading, so we don't need to use role.
//bool role = 0;

void setup() {
  pinMode(53, OUTPUT); //Required on Arduino MEGA
  Serial.begin(115200);

  radio.begin();

  // Set the PA Level low to prevent power supply related issues since this is a
  // getting_started sketch, and the likelihood of close proximity of the devices. RF24_PA_MAX is default.
  radio.setPALevel(RF24_PA_LOW);

  // Open a writing and reading pipe on each radio, with opposite addresses
  if (radioNumber) {
    radio.openWritingPipe(addresses[1]);
    radio.openReadingPipe(1, addresses[0]);
  } else {
    radio.openWritingPipe(addresses[0]);
    radio.openReadingPipe(1, addresses[1]);
  }

  // Start the radio listening for data
  radio.startListening();

  //Set motor pins to output
  pinMode(X_PWM_PIN, OUTPUT);
  pinMode(X_DIR1_PIN, OUTPUT);
  pinMode(Y_PWM_PIN, OUTPUT);
  pinMode(Y_DIR2_PIN, OUTPUT);

  pinMode(X_PWM_PIN2, OUTPUT);
  pinMode(X_DIR1_PIN2, OUTPUT);
  pinMode(Y_PWM_PIN2, OUTPUT);
  pinMode(Y_DIR2_PIN2, OUTPUT);

  //initialize servos and start them in the center
  SERVO1.attach(s1pin);
  SERVO2.attach(s2pin);
  SERVO1.write(85);
  SERVO2.write(102);
}

void loop() {


  /****************** Pong Back Role ***************************/

  unsigned long payload; // payload variable 4 bytes

  int motorY;     //Joystick 1 Y-axis
  int motorX;     //Joystick 1 X-axis
  int neckX;      // Joystick 2 X-axis
  int neckY;      // Joystick 2 Y-axis

  //servo variables
  int servo1angle;
  int servo2angle;
  
  if (radio.available()) {
    //Serial.print("Radio received!");
    // Variable for the received timestamp
    while (radio.available()) {                             // While there is data ready
      radio.read( &payload, sizeof(uint32_t) );             // Get the payload
      //Serial.println(payload);
    }

    // FOR USE IF CONTROLLER NEEDS FEEDBACK
    //radio.stopListening();                                        // First, stop listening so we can talk
    //radio.write( &payload, sizeof(unsigned long) );               // Send the final one back.
    //radio.startListening();                                       // Now, resume listening so we catch the next packets.

    //Decode our message into individual motor signals
    // Payload structure:
    //  ---- ---- | ---- ---- | ---- ---- | ---- ---- |
    //   Motor X     Motor Y     Neck X       Neck Y
    //-------------------------------------------------------

    motorX = (payload & 0xFF000000) >> 24; // Mask Everything except the Motor Back signal
    payload = payload;                     //Recopy payload (since we masked out all the other bits)
    motorY = (payload & 0x00FF0000) >> 16; // Mask Everything except the Motor Forward signal and shift it to the lowest bits
    payload = payload;
    neckX = (payload & 0x0000FF00) >> 8;
    payload = payload;
    neckY = payload & 0x000000FF;

    // Payload Debug Prints
    /*
    Serial.print("motorX: ");
    Serial.println(motorX);
    Serial.print("motorY: ");
    Serial.println(motorY);
    Serial.print("neckX: ");
    Serial.println(neckX);
    Serial.print("neckY: ");
    Serial.println(neckY);
    Serial.println();
    */
    
    // Joystick values can range -127 and +127, set 127 to 0 by subtracting 127
    motorY -= 127;
    motorX -= 127;
    neckY -= 127;
    neckX -= 127;

    // Modified to Zero Center Payload Debug Prints
    // values roughly (-127 <-> 127)
    /*
    Serial.print("motorX: ");
    Serial.println(motorX);
    Serial.print("motorY: ");
    Serial.println(motorY);
    Serial.print("neckX: ");
    Serial.println(neckX);
    Serial.print("neckY: ");
    Serial.println(neckY);
    Serial.println();
    */

    // Motor Damping Debug Prints
    /*
    Serial.print("motorY: ");
    Serial.println(motorY);
    Serial.print("currMotorY: ");
    Serial.println(currMotorY);

    Serial.print("motorX: ");
    Serial.println(motorX);
    Serial.print("currMotorX: ");
    Serial.println(currMotorX);

    Serial.println();
    */

    // scaling to account for overflow
    neckX = neckX / 2;
    neckY = neckY / 1.5;
    
    //SERVO OUTPUT CODE
    servo1angle = neckY;
    servo2angle = neckX;
    servo1angle = map(servo1angle, 127, -127, 55, 115);
    servo2angle = map(servo2angle, -127, 127, 72, 132);

    SERVO1.write(servo1angle); // write neckY
    SERVO2.write(servo2angle); // write neckX

    // MOTOR OUTPUT CODE
    // OPEN LOOP PROPORTIONAL CONTROLLER
        // Motor Y Direction and currMotorY update
        if (motorY < -5) {                   // if user input is negative Y
          //need to move backward (  )
          // and convert joystick angle to positive analog signal from 0 to 255
          if (currMotorY > 0){               // if was moving forward, then input suddenly changed to backwards
            currMotorY -= fastDampingConstant;
          }
          else if (currMotorY < motorY) {    // if user lowers input but not quite to zero
            currMotorY = motorY;
          }
          else {
            currMotorY -= dampingConstant;
            }
        }
        else if (motorY > 5) {               // if user input is positive Y
          //need to move forward (  )
          // and convert joystick angle to positive analog signal from 0 to 255
          if (currMotorY < 0){               // if was moving backwards, then input suddenly changed to forwards
            currMotorY += fastDampingConstant;
          }
          else if (currMotorY > motorY) {    // if user lowers input but not quite to zero
            currMotorY = motorY;
          }
          else {
            currMotorY += dampingConstant;
            }
        }
        else {    // user input is zero
          //need to release motor (  )
          // and set speed to 0
          if (currMotorY < 0 - fastDampingConstant) // damping from -Y to 0
            {
              currMotorY += fastDampingConstant;
             }
          else if (currMotorY > 0 + fastDampingConstant) // damping from +Y to 0
            {
              currMotorY -= fastDampingConstant;
             }
          else
            {                                    // if the output is less than fastDampingConstant just set to zero
             currMotorY = 0;
            }
        }


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
          
        

        // Motor X Direction and currMotorY update
        if (motorX < -5) {                   // if user input is negative X
          //need to move backward (  )
          // and convert joystick angle to positive analog signal from 0 to 255
          if (currMotorX > 0){               // if was moving forward, then input suddenly changed to backwards
            currMotorX -= fastDampingConstant;
          }
          else if (currMotorX < motorX) {    // if user lowers input but not quite to zero
            currMotorX = motorX;
          }
          else {
            currMotorX -= dampingConstant;
            }
        }
        else if (motorX > 5) {               // if user input is positive X
          //need to move forward (  )
          // and convert joystick angle to positive analog signal from 0 to 255
          if (currMotorX < 0){               // if was moving backwards, then input suddenly changed to forwards
            currMotorX += fastDampingConstant;
          }
          else if (currMotorX > motorX) {    // if user lowers input but not quite to zero
            currMotorX = motorX;
          }
          else {
            currMotorX += dampingConstant;
            }
        }
        else {    // user input is zero
          //need to release motor (  )
          // and set speed to 0
          if (currMotorX < 0 - fastDampingConstant)      // damping from -X to 0
            {currMotorX += fastDampingConstant;
             }
          else if (currMotorX > 0 + fastDampingConstant) // damping from +X to 0
            {currMotorX -= fastDampingConstant;
             }
          else {                                         // if the output is less than fastDampingConstant just set to zero
             currMotorX = 0;
              }
        }
        
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
          
        
      }

  //Serial.println("Radio not received :(");

} // Loop

//Motor 1 is janky
