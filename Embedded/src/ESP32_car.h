/*
ESP32 SMART CAR - SENSOR & DRIVER HEADERS
Sensors:
- Temperature Sensor (DS18B20)
- Voltage Detection Module (0-25V)
- Current + Power Sensor (INA219)
- Infrared Obstacle / Line Sensors (TCRT5000)
- MPU6050 Accelerometer + Gyroscope
- Ultrasonic Sensor (HC-SR04)
- Motor Driver TB6612FNG
- Encoders (From the Perseids Car Chassis)
- TFT Display (ST7735S)

SHPE Competitions FALL 2025

NOTE: This is a reference header file listing all required libraries.
Include this file in your main .ino sketch to get all sensor libraries.

Usage in your .ino file:
  #include "src/ESP32_car.h"
*/

#ifndef ESP32_CAR_H
#define ESP32_CAR_H

// ========== CORE ESP32 LIBRARIES ==========
#include <Arduino.h>              // Core ESP32 functions
#include <Wire.h>                 // I2C communication (SDA=GPIO21, SCL=GPIO22)
#include <SPI.h>                  // SPI communication (for TFT display)
#include <HardwareSerial.h>       // Serial communication for debugging

// ========== TEMPERATURE SENSOR (DS18B20) ==========
#include <OneWire.h>              // 1-Wire protocol library
#include <DallasTemperature.h>   // DS18B20 temperature sensor driver

// ========== VOLTAGE DETECTION MODULE (0-25V) ==========
#include <ESP32AnalogRead.h>      // Enhanced ADC reading for ESP32 (GPIO 34)

// ========== CURRENT + POWER SENSOR (INA219) ==========
#include <Adafruit_INA219.h>     // INA219 current and power sensor (I2C)

// ========== ACCELEROMETER + GYROSCOPE (MPU6050) ==========
#include <Adafruit_MPU6050.h>     // MPU6050 6-axis motion sensor driver
#include <Adafruit_Sensor.h>     // Adafruit sensor abstraction layer

// ========== ULTRASONIC SENSOR (HC-SR04) ==========
#include <NewPing.h>              // HC-SR04 ultrasonic distance sensor library

// ========== TFT DISPLAY (ST7735S) ==========
#include <Adafruit_GFX.h>         // Core graphics library
#include <Adafruit_ST7735.h>      // ST7735/ST7735S TFT display driver (SPI)

// ========== ENCODERS (QUADRATURE) ==========
#include <Encoder.h>              // Quadrature encoder library for motor encoders

// ========== INFRARED OBSTACLE / LINE SENSORS (TCRT5000) ==========
// No external library required - uses built-in digitalRead() and analogRead()
// Digital output: digitalRead(pin)
// Analog output: analogRead(pin) or ESP32AnalogRead

// ========== MOTOR DRIVER (TB6612FNG) ==========
// No external library required - uses ESP32 PWM functions
// Use ledcWrite() for PWM control (16 channels available on ESP32)

#endif // ESP32_CAR_H

