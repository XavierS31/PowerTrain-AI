/*
 * Simple test to verify DabbleESP32 library is working
 * Upload this to ESP32 and check Serial Monitor
 */

#define CUSTOM_SETTINGS
#define INCLUDE_GAMEPAD_MODULE
#include <DabbleESP32.h>

void setup() {
  Serial.begin(115200);
  Serial.println("Testing DabbleESP32 Library...");
  Serial.println("Library version check:");
  
  // Check if library compiles and includes correctly
  Serial.println("✓ DabbleESP32.h included successfully");
  Serial.println("✓ GamePad module available");
  
  // Initialize Dabble
  Dabble.begin("TestBluetoothCar");
  Serial.println("✓ Dabble initialized");
  Serial.println("\nWaiting for Bluetooth connection...");
  Serial.println("Connect via Dabble app on your phone!");
  Serial.println("Device name: TestBluetoothCar\n");
}

void loop() {
  Dabble.processInput();
  
  // Test button functions
  if (GamePad.isUpPressed()) {
    Serial.println("UP button pressed!");
  }
  if (GamePad.isDownPressed()) {
    Serial.println("DOWN button pressed!");
  }
  if (GamePad.isLeftPressed()) {
    Serial.println("LEFT button pressed!");
  }
  if (GamePad.isRightPressed()) {
    Serial.println("RIGHT button pressed!");
  }
  if (GamePad.isTrianglePressed()) {
    Serial.println("TRIANGLE button pressed!");
  }
  if (GamePad.isCrossPressed()) {
    Serial.println("CROSS button pressed!");
  }
  
  // Test joystick functions (if available)
  float xAxis = GamePad.getXaxisData();
  float yAxis = GamePad.getYaxisData();
  
  if (abs(xAxis) > 0.1 || abs(yAxis) > 0.1) {
    Serial.print("Joystick - X: ");
    Serial.print(xAxis);
    Serial.print(", Y: ");
    Serial.println(yAxis);
  }
  
  delay(100);
}

