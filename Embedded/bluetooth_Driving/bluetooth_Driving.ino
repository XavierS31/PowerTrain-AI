/*
 * Bluetooth Controlled Driving - Standalone Version
 * 
 * This is a minimal implementation for Bluetooth control of the car.
 * No autonomous driving, no data logging, no WiFi.
 * Just manual control via Bluetooth commands.
 * 
 * Usage:
 * 1. Upload this to ESP32
 * 2. Connect via Bluetooth (default name: "ESP32_Car")
 * 3. Send commands: F/B/L/R/S (Forward/Backward/Left/Right/Stop)
 * 4. Speed control: 1/2/3 (Low/Medium/High)
 */

#include <Arduino.h>
#include "BluetoothSerial.h"

#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error "Bluetooth is not enabled! Please run `make menuconfig` to enable it"
#endif

BluetoothSerial SerialBT;

// Motor Control
#define MOTOR_STBY 27
#define MOTOR_PWMA 25
#define MOTOR_AIN1 26
#define MOTOR_AIN2 13

// Speed Levels
#define SPEED_LOW 85
#define SPEED_MEDIUM 170
#define SPEED_HIGH 255

#define MIN_MOTOR_SPEED 50
const int PWMFreq = 20000;
const int PWMResolution = 8;
const int motorPWMSpeedChannel = 4;

int currentSpeed = SPEED_MEDIUM;
int currentRightSpeed = 0;
int currentLeftSpeed = 0;

// Motor Control Functions
void applyAcceleration(int targetRightSpeed, int targetLeftSpeed) {
  const int ACCEL_STEP = 5;
  const int DECEL_STEP = 8;
  
  if (targetRightSpeed > currentRightSpeed) {
    currentRightSpeed += ACCEL_STEP;
    if (currentRightSpeed > targetRightSpeed) currentRightSpeed = targetRightSpeed;
  } else if (targetRightSpeed < currentRightSpeed) {
    currentRightSpeed -= DECEL_STEP;
    if (currentRightSpeed < targetRightSpeed) currentRightSpeed = targetRightSpeed;
  }
  
  if (targetLeftSpeed > currentLeftSpeed) {
    currentLeftSpeed += ACCEL_STEP;
    if (currentLeftSpeed > targetLeftSpeed) currentLeftSpeed = targetLeftSpeed;
  } else if (targetLeftSpeed < currentLeftSpeed) {
    currentLeftSpeed -= DECEL_STEP;
    if (currentLeftSpeed < targetLeftSpeed) currentLeftSpeed = targetLeftSpeed;
  }
}

void rotateMotor(int rightMotorSpeed, int leftMotorSpeed) {
  applyAcceleration(rightMotorSpeed, leftMotorSpeed);
  
  int combinedSpeed = (currentRightSpeed + currentLeftSpeed) / 2;
  
  digitalWrite(MOTOR_STBY, HIGH);
  
  if (combinedSpeed < 0) {
    digitalWrite(MOTOR_AIN1, LOW);
    digitalWrite(MOTOR_AIN2, HIGH);
  } else if (combinedSpeed > 0) {
    digitalWrite(MOTOR_AIN1, HIGH);
    digitalWrite(MOTOR_AIN2, LOW);
  } else {
    digitalWrite(MOTOR_AIN1, LOW);
    digitalWrite(MOTOR_AIN2, LOW);
  }
  
  int pwmValue = abs(combinedSpeed);
  if (pwmValue > 0 && pwmValue < MIN_MOTOR_SPEED) {
    pwmValue = MIN_MOTOR_SPEED;
  }
  ledcWrite(motorPWMSpeedChannel, pwmValue);
}

void stopMotors() {
  rotateMotor(0, 0);
}

void driveForward(int speed) {
  rotateMotor(speed, speed);
}

void turnLeft(int speed) {
  rotateMotor(speed, -speed);
}

void turnRight(int speed) {
  rotateMotor(-speed, speed);
}

void driveBackward(int speed) {
  rotateMotor(-speed, -speed);
}

void setup() {
  Serial.begin(115200);
  delay(1000);
  
  Serial.println("========================================");
  Serial.println("  BLUETOOTH CONTROLLED DRIVING");
  Serial.println("========================================");
  Serial.println();
  
  // Initialize motor pins
  pinMode(MOTOR_AIN1, OUTPUT);
  pinMode(MOTOR_AIN2, OUTPUT);
  pinMode(MOTOR_PWMA, OUTPUT);
  pinMode(MOTOR_STBY, OUTPUT);
  digitalWrite(MOTOR_STBY, HIGH);
  digitalWrite(MOTOR_AIN1, LOW);
  digitalWrite(MOTOR_AIN2, LOW);
  
  // Setup PWM
  ledcSetup(motorPWMSpeedChannel, PWMFreq, PWMResolution);
  ledcAttachPin(MOTOR_PWMA, motorPWMSpeedChannel);
  
  // Initialize Bluetooth
  SerialBT.begin("ESP32_Car");
  Serial.println("Bluetooth initialized");
  Serial.println("Device name: ESP32_Car");
  Serial.println();
  Serial.println("Commands:");
  Serial.println("  F - Forward");
  Serial.println("  B - Backward");
  Serial.println("  L - Turn Left");
  Serial.println("  R - Turn Right");
  Serial.println("  S - Stop");
  Serial.println("  1 - Low speed");
  Serial.println("  2 - Medium speed");
  Serial.println("  3 - High speed");
  Serial.println();
  Serial.println("Waiting for Bluetooth connection...");
}

void loop() {
  // Check Bluetooth commands
  if (SerialBT.available()) {
    char cmd = SerialBT.read();
    Serial.print("Received: ");
    Serial.println(cmd);
    
    switch (cmd) {
      case 'F':
      case 'f':
        driveForward(currentSpeed);
        SerialBT.println("Forward");
        Serial.println("Forward");
        break;
        
      case 'B':
      case 'b':
        driveBackward(currentSpeed);
        SerialBT.println("Backward");
        Serial.println("Backward");
        break;
        
      case 'L':
      case 'l':
        turnLeft(currentSpeed);
        SerialBT.println("Turn Left");
        Serial.println("Turn Left");
        break;
        
      case 'R':
      case 'r':
        turnRight(currentSpeed);
        SerialBT.println("Turn Right");
        Serial.println("Turn Right");
        break;
        
      case 'S':
      case 's':
        stopMotors();
        SerialBT.println("Stop");
        Serial.println("Stop");
        break;
        
      case '1':
        currentSpeed = SPEED_LOW;
        SerialBT.println("Speed: LOW");
        Serial.println("Speed: LOW");
        break;
        
      case '2':
        currentSpeed = SPEED_MEDIUM;
        SerialBT.println("Speed: MEDIUM");
        Serial.println("Speed: MEDIUM");
        break;
        
      case '3':
        currentSpeed = SPEED_HIGH;
        SerialBT.println("Speed: HIGH");
        Serial.println("Speed: HIGH");
        break;
        
      default:
        SerialBT.println("Unknown command");
        Serial.println("Unknown command");
        break;
    }
  }
  
  // Also check Serial for debugging
  if (Serial.available()) {
    char cmd = Serial.read();
    
    switch (cmd) {
      case 'F':
      case 'f':
        driveForward(currentSpeed);
        Serial.println("Forward");
        break;
        
      case 'B':
      case 'b':
        driveBackward(currentSpeed);
        Serial.println("Backward");
        break;
        
      case 'L':
      case 'l':
        turnLeft(currentSpeed);
        Serial.println("Turn Left");
        break;
        
      case 'R':
      case 'r':
        turnRight(currentSpeed);
        Serial.println("Turn Right");
        break;
        
      case 'S':
      case 's':
        stopMotors();
        Serial.println("Stop");
        break;
        
      case '1':
        currentSpeed = SPEED_LOW;
        Serial.println("Speed: LOW");
        break;
        
      case '2':
        currentSpeed = SPEED_MEDIUM;
        Serial.println("Speed: MEDIUM");
        break;
        
      case '3':
        currentSpeed = SPEED_HIGH;
        Serial.println("Speed: HIGH");
        break;
        
      default:
        break;
    }
    
    while (Serial.available()) Serial.read();
  }
  
  delay(10);
}

