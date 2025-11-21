/*
TFT Display (ST7735S)
This file contains the code for the TFT display.
It displays "Car Driving Behavior Performance" with a 0-100% indicator.
It is connected to the ESP32 GPIO 5, 4, 2, 18, and 23.
*/

// Required Libraries
#include "../ESP32_car.h"
#include "TFT_LED.h"
#include <SPI.h>                  // SPI communication protocol
#include <Adafruit_GFX.h>         // Core graphics library
#include <Adafruit_ST7735.h>      // ST7735/ST7735S TFT display driver
#include <math.h>                 // For sqrt() in calculatePerformance()

// Pin Definitions
#define TFT_CS   5    // Chip Select
#define TFT_DC   4    // Data/Command
#define TFT_RST  2    // Reset
#define TFT_SCK  18   // SPI Clock
#define TFT_SDA  23   // SPI MOSI (Master Out Slave In)

// TFT Display Object
Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);

// Display dimensions (ST7735S typically 128x160)
#define TFT_WIDTH  128
#define TFT_HEIGHT 160

// Color definitions (16-bit RGB565)
#define BLACK   0x0000
#define BLUE    0x001F
#define RED     0xF800
#define GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF
#define ORANGE  0xFDA0

// Initialize TFT Display
void initTFT() {
  // Initialize SPI
  SPI.begin(TFT_SCK, -1, TFT_SDA, TFT_CS);
  
  // Initialize display
  tft.initR(INITR_BLACKTAB);  // Initialize for ST7735S chip
  tft.setRotation(0);          // 0 = portrait, 1 = landscape
  
  // Fill screen with black
  tft.fillScreen(BLACK);
  
  // Set text color and size
  tft.setTextColor(WHITE);
  tft.setTextSize(1);
  
  Serial.println("TFT Display initialized");
}

// Display Car Driving Behavior Performance (0-100%)
void displayPerformance(float performancePercent) {
  // Constrain percentage to 0-100
  if (performancePercent < 0) performancePercent = 0;
  if (performancePercent > 100) performancePercent = 100;
  
  // Clear screen
  tft.fillScreen(BLACK);
  
  // Title: "Car Driving Behavior Performance"
  tft.setTextColor(WHITE);
  tft.setTextSize(1);
  tft.setCursor(5, 5);
  tft.println("Car Driving");
  tft.setCursor(5, 15);
  tft.println("Behavior");
  tft.setCursor(5, 25);
  tft.println("Performance");
  
  // Draw percentage text (large)
  tft.setTextSize(3);
  tft.setTextColor(CYAN);
  
  // Center the percentage
  char percentStr[10];
  sprintf(percentStr, "%.0f%%", performancePercent);
  
  // Calculate center position for percentage
  int16_t x1, y1;
  uint16_t w, h;
  tft.getTextBounds(percentStr, 0, 0, &x1, &y1, &w, &h);
  int xPos = (TFT_WIDTH - w) / 2;
  int yPos = 60;
  
  tft.setCursor(xPos, yPos);
  tft.print(percentStr);
  
  // Draw progress bar background
  int barX = 10;
  int barY = 100;
  int barWidth = TFT_WIDTH - 20;
  int barHeight = 20;
  
  // Background rectangle (gray/black)
  tft.fillRect(barX, barY, barWidth, barHeight, BLACK);
  tft.drawRect(barX, barY, barWidth, barHeight, WHITE);
  
  // Calculate filled width based on percentage
  int filledWidth = (int)((performancePercent / 100.0) * barWidth);
  
  // Draw filled portion with color gradient
  uint16_t barColor;
  if (performancePercent < 33) {
    barColor = RED;      // Red for low performance
  } else if (performancePercent < 66) {
    barColor = YELLOW;   // Yellow for medium performance
  } else {
    barColor = GREEN;    // Green for high performance
  }
  
  if (filledWidth > 0) {
    tft.fillRect(barX + 1, barY + 1, filledWidth - 2, barHeight - 2, barColor);
  }
  
  // Status text at bottom
  tft.setTextSize(1);
  tft.setTextColor(WHITE);
  
  const char* status;
  if (performancePercent >= 80) {
    status = "Excellent";
    tft.setTextColor(GREEN);
  } else if (performancePercent >= 60) {
    status = "Good";
    tft.setTextColor(YELLOW);
  } else if (performancePercent >= 40) {
    status = "Fair";
    tft.setTextColor(ORANGE);
  } else {
    status = "Poor";
    tft.setTextColor(RED);
  }
  
  // Center status text
  tft.getTextBounds(status, 0, 0, &x1, &y1, &w, &h);
  xPos = (TFT_WIDTH - w) / 2;
  tft.setCursor(xPos, 130);
  tft.print(status);
}

// Update performance display (call this in your main loop)
void updatePerformanceDisplay(float performancePercent) {
  displayPerformance(performancePercent);
}

// Example usage function to calculate performance from sensor data
// You can customize this based on your metrics
float calculatePerformance(float voltage, float current, float distance, 
                         float accelX, float accelY, float accelZ) {
  float performance = 100.0;
  
  // Voltage factor (lower voltage = lower performance)
  if (voltage < 10.0) {
    performance -= (10.0 - voltage) * 5.0;  // Penalize low voltage
  }
  
  // Current factor (very high current = inefficiency)
  if (current > 5.0) {
    performance -= (current - 5.0) * 2.0;  // Penalize high current
  }
  
  // Distance factor (closer obstacles = lower performance)
  if (distance < 20.0) {
    performance -= (20.0 - distance) * 2.0;  // Penalize close obstacles
  }
  
  // Acceleration stability (smooth driving = better performance)
  float accelMagnitude = sqrt(accelX*accelX + accelY*accelY + accelZ*accelZ);
  if (accelMagnitude > 2.0) {  // High acceleration = rough driving
    performance -= (accelMagnitude - 2.0) * 5.0;
  }
  
  // Constrain to 0-100
  if (performance < 0) performance = 0;
  if (performance > 100) performance = 100;
  
  return performance;
}
