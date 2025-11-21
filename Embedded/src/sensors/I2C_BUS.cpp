/*
I2C Bus
This file contains the code for the I2C bus.
It is used to communicate with the sensors (INA219, MPU6050, etc.).
It is connected to the ESP32 GPIO 21 and 22.
*/

// Required Libraries
#include <Arduino.h>
#include <Wire.h>                  // I2C communication library

// Pin Definitions
#define I2C_SDA 21    // I2C Data line
#define I2C_SCL 22    // I2C Clock line