/*
ESP32 SMART CAR - SENSOR & DRIVER HEADERS
Sensors:
- Temperature Sensor (DS18B20)
- Voltage Detection Module (0-25V)
- Current + Power Sensor (INA219)
- Infrared Obstacle / Line Sensors (TCRT5000)
- MPU6050 Accelerometer + Gyroscope
- Motor Driver TB6612FNG
- Encoders (From the Perseids Car Chassis)

SHPE Competitions FALL 2025

*/
// -------- CORE ESP32 --------
#include <Arduino.h>
#include <Wire.h>

// -------- TEMPERATURE SENSOR (DS18B20) --------
#include <OneWire.h>
#include <DallasTemperature.h>

// -------- VOLTAGE DETECTION MODULE (0–25V) --------
// (Uses ESP32 ADC, no special library needed)
#include <ESP32AnalogRead.h>

// -------- CURRENT + POWER SENSOR (INA219) --------
#include <Adafruit_INA219.h>

// -------- INFRARED OBSTACLE / LINE SENSORS (TCRT5000) --------
// (Digital analog read — no library required)

// -------- MPU6050 ACCELEROMETER + GYROSCOPE --------
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>

// -------- MOTOR DRIVER TB6612FNG --------
// (We will control via GPIO PWM, no special library needed)

// -------- ENCODERS (FROM THE PERSEIDS CAR CHASSIS) --------
// If using quadrature encoders:
#include <Encoder.h>

// -------- OPTIONAL DEBUG USING SERIAL PLOTTER --------
#include <HardwareSerial.h>

/************************************************************/
