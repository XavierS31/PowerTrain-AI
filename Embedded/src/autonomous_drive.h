/*
 * Autonomous Driving Module Header
 * 
 * Pure autonomous driving functionality
 */

#ifndef AUTONOMOUS_DRIVE_H
#define AUTONOMOUS_DRIVE_H

#include <Arduino.h>

// Drive states
enum DriveState {
  DRIVING_FORWARD,
  TURNING_LEFT,
  TURNING_RIGHT,
  BACKING_UP,
  STOPPED
};

// ========== INITIALIZATION ==========
void initAutonomousDrive();

// ========== MAIN AUTONOMOUS FUNCTION ==========
void autonomousDrive();

// ========== MOTOR CONTROL ==========
void driveForward(int speed);
void turnLeft(int speed);
void turnRight(int speed);
void driveBackward(int speed);
void stopMotors();
void emergencyStop();

// ========== SPEED CONTROL ==========
void setSpeed(int speed);
void setSpeedLow();
void setSpeedMedium();
void setSpeedHigh();

// ========== STATUS ==========
DriveState getDriveState();
int getCurrentSpeed();
float getCurrentDistance();

#endif // AUTONOMOUS_DRIVE_H

