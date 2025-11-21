/*
IR_MH_Sensor (TCRT5000)
This file contains the code for the IR_MH_Sensor.
It is used to measure the distance of the object.
It is connected to the ESP32 GPIO 36 and 39.
*/

// Required Libraries
#include "../ESP32_car.h"
// Note: TCRT5000 uses built-in digitalRead() and analogRead()
// For better ADC accuracy, can use ESP32AnalogRead.h

// Pin Definitions
#define IR_DO 36    // Digital obstacle detect (digitalRead)
#define IR_AO 39    // Analog reflection level (analogRead)
