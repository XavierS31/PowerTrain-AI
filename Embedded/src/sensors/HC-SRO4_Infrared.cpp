/*
HC-SR04 Ultrasonic Sensor
This file contains the code for the HC-SR04 ultrasonic sensor.
It is used to measure the distance of the object.
It is connected to the ESP32 GPIO 17 and 16.
*/

// Required Libraries
#include "../ESP32_car.h"
#include <NewPing.h>               // HC-SR04 ultrasonic sensor library

// Pin Definitions
#define HC_TRIG 17    // Trigger pin
#define HC_ECHO 16    // Echo pin (through 10k/15k voltage divider for 3.3V ESP32)
