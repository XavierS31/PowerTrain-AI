/*
 * Pure Autonomous Driving - Standalone Version
 * 
 * This is a minimal implementation focused ONLY on autonomous driving.
 * No data logging, no WiFi, no TFT display updates.
 * Just pure obstacle avoidance and navigation.
 * 
 * Usage:
 * 1. Upload this to ESP32
 * 2. Car will start driving autonomously immediately
 * 3. Use Serial commands to control speed: L/M/H/S (Low/Medium/High/Stop)
 */

#include <Arduino.h>
#include "ESP32_car.h"
#include "config/config.h"
#include <NewPing.h>
#include <Encoder.h>

// ========== MOTOR CONTROL ==========
#define MOTOR_STBY 27
#define MOTOR_PWMA 25
#define MOTOR_AIN1 26
#define MOTOR_AIN2 13

// Speed Levels
#define SPEED_LOW MOTOR_SPEED_LOW
#define SPEED_MEDIUM MOTOR_SPEED_MEDIUM
#define SPEED_HIGH MOTOR_SPEED_HIGH

#define MIN_MOTOR_SPEED 50
const int PWMFreq = 20000;
const int PWMResolution = 8;
const int motorPWMSpeedChannel = 4;

int currentSpeed = SPEED_MEDIUM;
int currentRightSpeed = 0;
int currentLeftSpeed = 0;

// ========== SENSOR PINS ==========
#define HC_TRIG 17
#define HC_ECHO 16
#define IR_DO 36
#define IR_AO 39

// ========== OBSTACLE DETECTION ==========
NewPing sonar(HC_TRIG, HC_ECHO, 200);
#define OBSTACLE_DISTANCE OBSTACLE_DISTANCE_CM
#define TURN_DURATION TURN_DURATION_MS
#define SAFE_DISTANCE 40  // cm - start slowing down
#define CRITICAL_DISTANCE 20  // cm - immediate stop

// ========== MOTOR CONTROL FUNCTIONS ==========
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

// ========== SENSOR READING ==========
float readDistance() {
  float distance = sonar.ping_cm();
  if (distance == 0) distance = 200;
  return distance;
}

bool readIRSensor() {
  return digitalRead(IR_DO) == LOW;
}

float checkLeftDistance() {
  turnLeft(currentSpeed / 2);
  delay(200);
  float distance = readDistance();
  stopMotors();
  delay(50);
  return distance;
}

float checkRightDistance() {
  turnRight(currentSpeed / 2);
  delay(200);
  float distance = readDistance();
  stopMotors();
  delay(50);
  return distance;
}

// ========== AUTONOMOUS DRIVING ==========
void autonomousDrive() {
  float distance = readDistance();
  bool irObstacle = readIRSensor();
  
  // Critical obstacle - immediate stop and backup
  if (distance < CRITICAL_DISTANCE || irObstacle) {
    stopMotors();
    delay(200);
    
    driveBackward(currentSpeed / 2);
    delay(300);
    stopMotors();
    delay(100);
    
    // Check which direction is clearer
    float leftDistance = checkLeftDistance();
    float rightDistance = checkRightDistance();
    
    if (leftDistance > rightDistance && leftDistance > OBSTACLE_DISTANCE) {
      turnLeft(currentSpeed);
      delay(TURN_DURATION);
    } else if (rightDistance > OBSTACLE_DISTANCE) {
      turnRight(currentSpeed);
      delay(TURN_DURATION);
    } else {
      // Both sides blocked, back up more and turn
      driveBackward(currentSpeed / 2);
      delay(500);
      turnRight(currentSpeed);
      delay(TURN_DURATION * 2);
    }
    return;
  }
  
  // Obstacle approaching - slow down
  if (distance < SAFE_DISTANCE && distance >= OBSTACLE_DISTANCE) {
    int reducedSpeed = map(distance, OBSTACLE_DISTANCE, SAFE_DISTANCE, 
                          currentSpeed / 3, currentSpeed);
    driveForward(reducedSpeed);
    return;
  }
  
  // Obstacle detected - turn away
  if (distance < OBSTACLE_DISTANCE) {
    stopMotors();
    delay(100);
    
    float leftDistance = checkLeftDistance();
    float rightDistance = checkRightDistance();
    
    if (leftDistance > rightDistance && leftDistance > OBSTACLE_DISTANCE) {
      turnLeft(currentSpeed);
      delay(TURN_DURATION);
    } else if (rightDistance > OBSTACLE_DISTANCE) {
      turnRight(currentSpeed);
      delay(TURN_DURATION);
    } else {
      // Both sides blocked
      driveBackward(currentSpeed / 2);
      delay(400);
      turnRight(currentSpeed);
      delay(TURN_DURATION * 1.5);
    }
    return;
  }
  
  // Clear path - drive forward
  driveForward(currentSpeed);
}

// ========== SETUP ==========
void setup() {
  Serial.begin(115200);
  delay(1000);
  
  Serial.println("========================================");
  Serial.println("  AUTONOMOUS DRIVING MODE");
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
  
  // Initialize IR sensor
  pinMode(IR_DO, INPUT);
  pinMode(IR_AO, INPUT);
  
  Serial.println("Autonomous driving initialized");
  Serial.print("Speed: ");
  Serial.println(currentSpeed);
  Serial.println();
  Serial.println("Commands:");
  Serial.println("  L - Low speed");
  Serial.println("  M - Medium speed (default)");
  Serial.println("  H - High speed");
  Serial.println("  S - Stop");
  Serial.println();
  Serial.println("Starting autonomous driving in 2 seconds...");
  delay(2000);
  
  Serial.println("GO!");
}

// ========== MAIN LOOP ==========
void loop() {
  // Check for serial commands
  if (Serial.available()) {
    char cmd = Serial.read();
    
    switch (cmd) {
      case 'L':
      case 'l':
        currentSpeed = SPEED_LOW;
        Serial.println("Speed: LOW");
        break;
        
      case 'M':
      case 'm':
        currentSpeed = SPEED_MEDIUM;
        Serial.println("Speed: MEDIUM");
        break;
        
      case 'H':
      case 'h':
        currentSpeed = SPEED_HIGH;
        Serial.println("Speed: HIGH");
        break;
        
      case 'S':
      case 's':
        stopMotors();
        Serial.println("STOPPED");
        break;
        
      case 'G':
      case 'g':
        driveForward(currentSpeed);
        Serial.println("Resuming...");
        break;
        
      default:
        break;
    }
    
    while (Serial.available()) Serial.read();
  }
  
  // Run autonomous driving
  autonomousDrive();
  
  delay(10);
}

