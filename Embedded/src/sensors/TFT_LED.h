/*
TFT Display (ST7735S) Header
Functions to display Car Driving Behavior Performance on TFT screen
*/

#ifndef TFT_LED_H
#define TFT_LED_H

#include "../ESP32_car.h"

// Initialize TFT Display
void initTFT();

// Display Car Driving Behavior Performance (0-10 level with bar)
void displayPerformance(float performancePercent, float rint, float voltage, float current, float temperature = 25.0);

// Update performance display (call this in your main loop)
void updatePerformanceDisplay(float performancePercent, float rint, float voltage, float current, float temperature = 25.0);

// Calculate performance from sensor data
float calculatePerformance(float voltage, float current, float distance, 
                         float accelX, float accelY, float accelZ);

// Get the calculated Rint value
float getCalculatedRint();

#endif // TFT_LED_H

