/*
Voltage Sensor (0-25V Voltage Detection Module)
This file contains the code for the voltage sensor.
It is used to measure the voltage of the battery.
It is connected to the ESP32 GPIO 34.
It is a simple voltage divider circuit.
*/

// Required Libraries
#include <Arduino.h>
#include <ESP32AnalogRead.h>  // Enhanced ADC reading for ESP32

// Pin Definition
#define VOLTAGE_SENSE_PIN 34

