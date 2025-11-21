/*
TFT Display (ST7735S)
This file contains the code for the TFT display.
It is used to display the data on the TFT screen.
It is connected to the ESP32 GPIO 5, 4, 2, 18, and 23.
*/

// Required Libraries
#include <Arduino.h>
#include <SPI.h>                  // SPI communication protocol
#include <Adafruit_GFX.h>         // Core graphics library
#include <Adafruit_ST7735.h>      // ST7735/ST7735S TFT display driver

// Pin Definitions
#define TFT_CS   5    // Chip Select
#define TFT_DC   4    // Data/Command
#define TFT_RST  2    // Reset
#define TFT_SCK  18   // SPI Clock
#define TFT_SDA  23   // SPI MOSI (Master Out Slave In)
