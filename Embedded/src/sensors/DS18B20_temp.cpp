/*
DS18B20 Temperature Sensor
This file contains the code for the DS18B20 temperature sensor.
It is used to measure the temperature of the motor.
It is connected to the ESP32 GPIO 15.
*/

// Required Libraries
#include <Arduino.h>
#include <OneWire.h>
#include <DallasTemperature.h>

// Pin Definition
#define DS18B20_PIN 15

