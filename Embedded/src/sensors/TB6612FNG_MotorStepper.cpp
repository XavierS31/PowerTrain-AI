/*
TB6612FNG Motor Driver
This file contains the pin definitions for the TB6612FNG motor driver.
Two separate TB6612FNG drivers are used, each controlling 2 motors in series.
Both drivers share the same control pins (AIN1, AIN2, PWMA, STBY).

TB6612FNG Specifications:
- Dual H-bridge motor driver
- PWM frequency: up to 100 KHz
- Direction control via AIN1/AIN2 (or BIN1/BIN2 for Channel B)
- STBY pin: HIGH=active, LOW=standby
- Output terminals: AO1/AO2 (Channel A), BO1/BO2 (Channel B)
*/

// Required Libraries
#include "../ESP32_car.h"
// Note: TB6612FNG uses ESP32 PWM functions (ledcWrite)
// No external library required - uses built-in ESP32 PWM

// TB6612FNG Pin Definitions
// Shared Control Pins (both TB6612FNG drivers use these same pins)
#define MOTOR_STBY 27   // Standby/Enable pin (HIGH=active, LOW=standby)
#define MOTOR_PWMA 25   // PWM speed control (PWMA) - 0-255 for 8-bit resolution
#define MOTOR_AIN1 26   // Direction control pin 1 (AIN1)
#define MOTOR_AIN2 13   // Direction control pin 2 (AIN2)

// TB6612FNG Direction Control Truth Table:
// AIN1=LOW,  AIN2=LOW  → Stop (short brake)
// AIN1=LOW,  AIN2=HIGH → CCW (Counter-Clockwise / Reverse)
// AIN1=HIGH, AIN2=LOW  → CW  (Clockwise / Forward)
// AIN1=HIGH, AIN2=HIGH → Stop (short brake)

// Motor Connections:
// Right TB6612FNG: 2 motors in series → connected to AO1 and AO2 outputs
// Left TB6612FNG:  2 motors in series → connected to AO1 and AO2 outputs
