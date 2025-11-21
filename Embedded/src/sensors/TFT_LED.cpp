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
#define BLACK     0x0000
#define WHITE     0xFFFF
#define RED       0xF800
#define GREEN     0x07E0
#define DARK_GREEN 0x03E0
#define LIGHT_GREEN 0x87F0
#define BLUE      0x001F
#define DARK_BLUE 0x0010
#define CYAN      0x07FF
#define GRAY      0x8410
#define DARK_GRAY 0x4208
#define LIGHT_GRAY 0xC618
#define ORANGE    0xFDA0
#define AMBER     0xFF80
#define TEAL      0x0410
#define NAVY      0x000F

// Initialize TFT Display
void initTFT() {
  Serial.println("Initializing TFT Display...");
  
  // Initialize SPI with proper ESP32 pins
  // ESP32 SPI: VSPI (default) uses pins: MOSI=23, MISO=19, SCK=18, CS=5
  // We're using: CS=5, DC=4, RST=2, SCK=18, MOSI=23
  
  // Set up RST pin
  pinMode(TFT_RST, OUTPUT);
  digitalWrite(TFT_RST, HIGH);
  delay(10);
  
  // Reset the display
  digitalWrite(TFT_RST, LOW);
  delay(50);
  digitalWrite(TFT_RST, HIGH);
  delay(100);
  
  // Initialize SPI
  SPI.begin(TFT_SCK, -1, TFT_SDA, TFT_CS);  // SCK, MISO (unused), MOSI, CS
  delay(100);
  
  // Try different initialization modes in order
  // Some ST7735S displays need specific initialization
  Serial.println("Trying INITR_BLACKTAB...");
  tft.initR(INITR_BLACKTAB);
  delay(100);
  
  tft.setRotation(0);
  tft.fillScreen(BLACK);
  delay(50);
  
  // Test if display works by drawing colored rectangles
  tft.fillRect(0, 0, 20, 20, RED);
  delay(100);
  tft.fillRect(20, 0, 20, 20, GREEN);
  delay(100);
  tft.fillRect(40, 0, 20, 20, BLUE);
  delay(100);
  
  // Clear and set up for normal operation
  tft.fillScreen(BLACK);
  delay(50);
  
  // Set text color and size
  tft.setTextColor(WHITE);
  tft.setTextSize(1);
  
  Serial.println("TFT Display initialized (using INITR_BLACKTAB)");
  Serial.println("If display is still white, check wiring and power connections");
}

// Display Car Driving Behavior Performance (0-100%)
// Shows hardcoded static values - no sensor data dependency
void displayPerformance(float performancePercent, float rint, float voltage, float current) {
  // HARDCODED VALUES - No actual measurements used
  performancePercent = 50.0;  // Fixed 50% performance
  rint = 0.7;                 // Fixed 0.7 ohm (realistic internal resistance for AA batteries)
  voltage = 6.0;              // Fixed 6.0V (4-pack 1.5V AA batteries = 6V)
  float temperature = 30.0;   // Fixed 30°C (realistic temp for AA batteries after mid-use)
  
  // Constrain percentage to 0-100
  if (performancePercent < 0) performancePercent = 0;
  if (performancePercent > 100) performancePercent = 100;
  
  // Always clear and redraw to ensure display updates
  tft.fillScreen(BLACK);
  delay(10);  // Small delay to ensure screen clear
  
  // Title: "Car Driving Performance" - CENTERED
  tft.setTextColor(CYAN);
  tft.setTextSize(1);
  
  // Center the title text
  int16_t x1, y1;
  uint16_t w, h;
  tft.getTextBounds("Car Driving Performance", 0, 0, &x1, &y1, &w, &h);
  int titleX = (TFT_WIDTH - w) / 2;
  tft.setCursor(titleX, 5);
  tft.println("Car Driving Performance");
  
  // Draw three boxes side by side for Rint, Voltage, Temperature
  int boxWidth = 38;
  int boxHeight = 30;
  int boxSpacing = 2;
  int startX = 4;
  int startY = 25;  // Moved up since no percentage at top
  
  // Box 1: Internal Resistance (Rint)
  tft.drawRect(startX, startY, boxWidth, boxHeight, TEAL);
  tft.fillRect(startX + 1, startY + 1, boxWidth - 2, boxHeight - 2, DARK_GRAY);
  tft.setTextColor(CYAN);
  tft.setTextSize(1);
  tft.setCursor(startX + 2, startY + 2);
  tft.println("Rint");
  char rintStr[10];
  if (rint < 0.001) {
    sprintf(rintStr, "0.00");
  } else if (rint < 1.0) {
    sprintf(rintStr, "%.2f", rint);
  } else {
    sprintf(rintStr, "%.1f", rint);
  }
  tft.setTextColor(WHITE);
  tft.setTextSize(1);
  tft.setCursor(startX + 2, startY + 12);
  tft.print(rintStr);
  tft.setTextColor(GRAY);
  tft.setTextSize(1);
  tft.setCursor(startX + 2, startY + 22);
  tft.print("Ohm");
  
  // Box 2: Voltage
  int box2X = startX + boxWidth + boxSpacing;
  tft.drawRect(box2X, startY, boxWidth, boxHeight, AMBER);
  tft.fillRect(box2X + 1, startY + 1, boxWidth - 2, boxHeight - 2, DARK_GRAY);
  tft.setTextColor(AMBER);
  tft.setTextSize(1);
  tft.setCursor(box2X + 2, startY + 2);
  tft.println("Volt");
  char voltStr[10];
  sprintf(voltStr, "%.1f", voltage);
  tft.setTextColor(WHITE);
  tft.setTextSize(1);
  tft.setCursor(box2X + 2, startY + 12);
  tft.print(voltStr);
  tft.setTextColor(GRAY);
  tft.setTextSize(1);
  tft.setCursor(box2X + 2, startY + 22);
  tft.print("V");
  
  // Box 3: Temperature (replaced Current)
  int box3X = box2X + boxWidth + boxSpacing;
  tft.drawRect(box3X, startY, boxWidth, boxHeight, LIGHT_GREEN);
  tft.fillRect(box3X + 1, startY + 1, boxWidth - 2, boxHeight - 2, DARK_GRAY);
  tft.setTextColor(LIGHT_GREEN);
  tft.setTextSize(1);
  tft.setCursor(box3X + 2, startY + 2);
  tft.println("Temp");
  char tempStr[10];
  sprintf(tempStr, "%.0f", temperature);
  tft.setTextColor(WHITE);
  tft.setTextSize(1);
  tft.setCursor(box3X + 2, startY + 12);
  tft.print(tempStr);
  tft.setTextColor(GRAY);
  tft.setTextSize(1);
  tft.setCursor(box3X + 2, startY + 22);
  tft.print("°C");
  
  // Draw progress bar background (underneath the boxes)
  int barX = 10;
  int barY = 63;  // Moved up since boxes are higher
  int barWidth = TFT_WIDTH - 20;
  int barHeight = 18;
  
  // Background rectangle with subtle border
  tft.fillRect(barX, barY, barWidth, barHeight, DARK_GRAY);
  tft.drawRect(barX, barY, barWidth, barHeight, GRAY);
  
  // Calculate filled width based on percentage
  int filledWidth = (int)((performancePercent / 100.0) * (barWidth - 2));
  
  // Draw filled portion in GREEN with gradient effect
  if (filledWidth > 0) {
    // Main green fill
    tft.fillRect(barX + 1, barY + 1, filledWidth, barHeight - 2, GREEN);
    
    // Add subtle highlight at top for depth
    if (filledWidth > 3) {
      tft.drawLine(barX + 1, barY + 1, barX + filledWidth, barY + 1, LIGHT_GREEN);
    }
  }
  
  // Percentage and Status text at bottom - together
  // For 50% performance, status is "Fair"
  const char* status;
  uint16_t statusColor;
  if (performancePercent >= 80) {
    status = "Excellent";
    statusColor = LIGHT_GREEN;
  } else if (performancePercent >= 60) {
    status = "Good";
    statusColor = CYAN;
  } else if (performancePercent >= 40) {
    status = "Fair";  // 50% falls here
    statusColor = AMBER;
  } else {
    status = "Poor";
    statusColor = ORANGE;
  }
  
  // Create combined string: "85% Excellent"
  char combinedStr[25];
  sprintf(combinedStr, "%.0f%% %s", performancePercent, status);
  
  // Center the combined text
  tft.setTextColor(statusColor);
  tft.setTextSize(2);  // Larger text for percentage and status
  tft.getTextBounds(combinedStr, 0, 0, &x1, &y1, &w, &h);
  int combinedX = (TFT_WIDTH - w) / 2;
  tft.setCursor(combinedX, 88);  // Position below progress bar
  tft.print(combinedStr);
}

// Update performance display (call this in your main loop)
void updatePerformanceDisplay(float performancePercent, float rint, float voltage, float current) {
  // Prevent TFT from turning off by refreshing display
  // This ensures the screen stays on
  displayPerformance(performancePercent, rint, voltage, current);
  
  // Optional: Send a keep-alive signal to prevent display sleep
  // Some displays have auto-sleep, this prevents it
}

/*
 * COMMENTED OUT - Actual measurement calculations
 * 
 * calculatePerformance() - Calculates Car Driving Behavior Performance (0-100%)
 * 
 * This function uses Internal Resistance (Rint) formula to assess battery health:
 * 
 * Internal Resistance Formula:
 *   Rint = (V_open - V_loaded) / I
 *   Or: Rint = ΔV / I (voltage drop under load)
 * 
 * For battery health:
 *   - Lower Rint = Better battery health = Higher performance
 *   - Higher Rint = Degraded battery = Lower performance
 * 
 * Performance calculation:
 *   1. Calculate internal resistance from voltage and current
 *   2. Compare to baseline (healthy battery Rint ~0.05-0.1 ohms for small batteries)
 *   3. Factor in driving smoothness (acceleration)
 *   4. Factor in safety (obstacle distance)
 * 
 * The value is calculated based on Rint and other factors.
 * Final value is constrained to 0-100% range.
 */
// Global variable to store calculated Rint (so we can display it)
// COMMENTED OUT - Not using actual measurements
// float calculatedRint = 0.0;

/*
float calculatePerformance(float voltage, float current, float distance, 
                         float accelX, float accelY, float accelZ) {
  // Internal Resistance Calculation
  // Rint = Voltage drop / Current
  // For loaded battery: Rint ≈ (V_nominal - V_actual) / I
  // Or simpler: Rint = V / I (when under load)
  
  // Nominal battery voltage (adjust based on your battery - e.g., 12V, 7.4V, etc.)
  const float V_NOMINAL = 12.0;  // Adjust to your battery's nominal voltage
  
  // Calculate internal resistance (ohms)
  // When current is flowing, voltage drops due to internal resistance
  calculatedRint = 0.0;
  if (current > 0.01) {  // Avoid division by zero
    // Method 1: Using voltage drop under load
    float voltageDrop = V_NOMINAL - voltage;
    calculatedRint = voltageDrop / current;
    
    // If voltage drop is negative or very small, use alternative method
    if (voltageDrop < 0 || calculatedRint < 0) {
      // Method 2: Direct calculation (R = V/I when under load)
      // This gives effective resistance including internal resistance
      calculatedRint = voltage / current;
    }
  } else {
    // No current flow, assume good condition
    calculatedRint = 0.0;
  }
  
  // Baseline internal resistance for healthy battery (adjust based on your battery)
  // Typical values: Small LiPo ~0.05-0.1Ω, Lead-acid ~0.01-0.05Ω
  const float RINT_BASELINE = 0.05;  // 50 mΩ baseline (adjust to your battery)
  const float RINT_MAX = 0.5;        // Maximum acceptable Rint (500 mΩ)
  
  // Calculate performance based on internal resistance
  // Lower Rint = better performance
  float performance = 100.0;
  
  if (calculatedRint > 0) {
    // Performance decreases as Rint increases
    // Linear scale: Rint_baseline = 100%, Rint_max = 0%
    if (calculatedRint <= RINT_BASELINE) {
      performance = 100.0;  // Excellent - Rint at or below baseline
    } else if (calculatedRint >= RINT_MAX) {
      performance = 0.0;    // Critical - Rint too high
    } else {
      // Linear interpolation between baseline and max
      float rintRange = RINT_MAX - RINT_BASELINE;
      float rintExcess = calculatedRint - RINT_BASELINE;
      performance = 100.0 * (1.0 - (rintExcess / rintRange));
    }
  }
  
  // Additional factors that affect overall performance
  
  // Voltage factor (very low voltage = critical, regardless of Rint)
  if (voltage < 9.0) {
    performance *= 0.5;  // Severe penalty for very low voltage
  } else if (voltage < 10.0) {
    performance *= 0.8;  // Moderate penalty for low voltage
  }
  
  // Current factor (very high current = stress on battery)
  if (current > 10.0) {
    performance -= 10.0;  // Penalty for high current draw
  } else if (current > 5.0) {
    performance -= 5.0;   // Small penalty for moderate current
  }
  
  // Distance factor (closer obstacles = lower safety performance)
  if (distance < 10.0) {
    performance -= 15.0;  // Critical - very close obstacle
  } else if (distance < 20.0) {
    performance -= 10.0;  // Warning - close obstacle
  } else if (distance < 30.0) {
    performance -= 5.0;   // Caution - approaching obstacle
  }
  
  // Acceleration stability (smooth driving = better overall performance)
  float accelMagnitude = sqrt(accelX*accelX + accelY*accelY + accelZ*accelZ);
  if (accelMagnitude > 3.0) {  // Very rough driving
    performance -= 10.0;
  } else if (accelMagnitude > 2.0) {  // Rough driving
    performance -= 5.0;
  }
  
  // Constrain to 0-100
  if (performance < 0) performance = 0;
  if (performance > 100) performance = 100;
  
  // Debug output (optional - can be removed)
  static unsigned long lastDebug = 0;
  if (millis() - lastDebug > 2000) {  // Print every 2 seconds
    Serial.print("Rint: ");
    Serial.print(calculatedRint, 4);
    Serial.print("Ω, V: ");
    Serial.print(voltage, 2);
    Serial.print("V, I: ");
    Serial.print(current, 2);
    Serial.print("A, Performance: ");
    Serial.print(performance, 1);
    Serial.println("%");
    lastDebug = millis();
  }
  
  return performance;
}
*/

// COMMENTED OUT - Not using actual measurements
// Get the calculated Rint value for display
/*
float getCalculatedRint() {
  return calculatedRint;
}
*/

// Return hardcoded Rint value for compatibility
float getCalculatedRint() {
  return 0.10;  // Hardcoded 0.10 ohm
}
