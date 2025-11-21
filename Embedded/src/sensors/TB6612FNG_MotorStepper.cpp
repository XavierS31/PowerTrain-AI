/*
TB6612FNG Motor Driver
This file contains the code for the TB6612FNG motor driver.
It is used to control the motor.
It is connected to the ESP32 GPIO 27, 25, 26, and 13.
*/

// Required Libraries
#include <Arduino.h>
// Note: TB6612FNG uses ESP32 PWM functions (ledcWrite)
// No external library required - uses built-in ESP32 PWM

// Pin Definitions
#define MOTOR_STBY 27   // Standby/Enable drivers (digitalWrite)
#define MOTOR_PWMA 25   // Speed PWM (ledcWrite for ESP32)
#define MOTOR_AIN1 26   // Direction 1 (digitalWrite)
#define MOTOR_AIN2 13   // Direction 2 (digitalWrite)
