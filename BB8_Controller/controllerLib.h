#ifndef CONTROLLER_H
#define CONTROLLER_H

// include libraries that make our lives easier
#include <SPI.h>  // SPI is protocol Arduino uses to communicate with RF chip
#include "RF24.h" // contains functions that help us use the RF chip
#include <Arduino.h>


// sets pin modes for each peripheral
void setPins();

// serial message debug printing
void printMessage(uint8_t message[]);

#endif // 
