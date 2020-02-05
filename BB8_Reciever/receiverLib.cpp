#include "receiverLib.h"

void setPins()
{
  pinMode(53, OUTPUT); //Required on Arduino MEGA
  
  //Set motor pins to output
  pinMode(X_PWM_PIN, OUTPUT);
  pinMode(X_DIR1_PIN, OUTPUT);
  pinMode(Y_PWM_PIN, OUTPUT);
  pinMode(Y_DIR2_PIN, OUTPUT);
  
  pinMode(X_PWM_PIN2, OUTPUT);
  pinMode(X_DIR1_PIN2, OUTPUT);
  pinMode(Y_PWM_PIN2, OUTPUT);
  pinMode(Y_DIR2_PIN2, OUTPUT);
  
  // set all the rotation motor control pins to outputs
  pinMode(rotEn, OUTPUT);
  pinMode(rotDir1, OUTPUT);
  pinMode(rotDir2, OUTPUT);
}

void printPayload(uint8_t payload[])
{
      // Payload Debug Prints   
    Serial.print("motorX: ");
    Serial.println(payload[0]);
    Serial.print("motorY: ");
    Serial.println(payload[1]);
    Serial.print("neckX: ");
    Serial.println(payload[2]);
    Serial.print("neckY: ");
    Serial.println(payload[3]);
    Serial.print("rotate: ");
    Serial.println(payload[4]);
    Serial.println();
}

int negativeInputDamp(int input, int curr)
{
  // need to move backward
  // and convert joystick angle to positive analog signal from 0 to 255
  if (curr > 0){               // if was moving forward, then input suddenly changed to backwards
    curr -= fastDampingConstant;
  }
  else if (curr < input) {    // if user lowers input but not quite to zero
    curr = input;
  }
  else {
    curr -= dampingConstant;
    }
  return curr;
}

int positiveInputDamp(int input, int curr)
{
  // need to move forward
  // and convert joystick angle to positive analog signal from 0 to 255
  if (curr < 0){               // if was moving backwards, then input suddenly changed to forwards
    curr += fastDampingConstant;
  }
  else if (curr > input) {    // if user lowers input but not quite to zero
    curr = input;
  }
  else {
    curr += dampingConstant;
    }
  return curr;
}

int zeroInputDamp(int input, int curr)
{
  //need to release motor (  )
  // and set speed to 0
  if (curr < 0 - fastDampingConstant) // damping from -Y to 0
    {
      curr += fastDampingConstant;
     }
  else if (curr > 0 + fastDampingConstant) // damping from +Y to 0
    {
      curr -= fastDampingConstant;
     }
  else
    {                                    // if the output is less than fastDampingConstant just set to zero
     curr = 0;
    }
  return curr;
}
