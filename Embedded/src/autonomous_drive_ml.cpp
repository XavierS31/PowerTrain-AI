/*
 * ML-Enhanced Autonomous Driving
 * 
 * This file shows how to integrate ML predictions into autonomous driving.
 * The ML model predicts performance level (1-10) based on sensor features,
 * and the car adjusts its behavior accordingly.
 */

#include "autonomous_drive.h"
#include "ml_predictor.h"
#include "ESP32_car.h"
#include "config/config.h"

// Server URL for ML predictions
const char* ML_SERVER_URL = "Server_API";  // Update with your server IP

// ML prediction state
static MLPrediction lastPrediction;
static unsigned long lastMLUpdate = 0;
static const unsigned long ML_UPDATE_INTERVAL = 2000;  // Update every 2 seconds

// Speed adjustment based on ML prediction
static int baseSpeed = 80;  // Base speed (0-255)
static int currentMLAdjustedSpeed = baseSpeed;

/**
 * Get ML prediction and adjust speed accordingly
 * This is called periodically to update behavior based on predicted performance
 * 
 * NOTE: You need to implement readSensors() function that returns SensorData
 * with fields: voltage, current, temperature, accelX, accelY, accelZ, distance, speed
 */
void updateMLPrediction(SensorData sensorData) {
    unsigned long now = millis();
    
    // Only update prediction every ML_UPDATE_INTERVAL milliseconds
    if (now - lastMLUpdate < ML_UPDATE_INTERVAL) {
        return;
    }
    
    lastMLUpdate = now;
    
    // Get prediction from Python server
    lastPrediction = getMLPrediction(sensorData, ML_SERVER_URL);
    
    if (lastPrediction.success) {
        // Adjust speed based on predicted performance level (1-10)
        // Level 1 = very poor performance -> slow down
        // Level 10 = excellent performance -> can go faster
        
        int speedAdjustment = (lastPrediction.level - 5) * 10;  // -40 to +50
        currentMLAdjustedSpeed = baseSpeed + speedAdjustment;
        
        // Apply confidence penalty - if low confidence, be more cautious
        if (lastPrediction.confidence < 0.5) {
            currentMLAdjustedSpeed = (int)(currentMLAdjustedSpeed * 0.7);
        }
        
        // Clamp speed to safe range
        currentMLAdjustedSpeed = constrain(currentMLAdjustedSpeed, 30, 100);
        
        Serial.print("ML Level: ");
        Serial.print(lastPrediction.level);
        Serial.print(", Confidence: ");
        Serial.print(lastPrediction.confidence);
        Serial.print(", Adjusted Speed: ");
        Serial.println(currentMLAdjustedSpeed);
    } else {
        // If prediction fails, use base speed
        currentMLAdjustedSpeed = baseSpeed;
        Serial.print("ML prediction failed: ");
        Serial.println(lastPrediction.error_message);
    }
}

/**
 * ML-Enhanced Autonomous Drive
 * 
 * This function integrates ML predictions into the autonomous driving logic.
 * The car adjusts its speed and behavior based on predicted performance level.
 */
void autonomousDriveML() {
    // Use ML-adjusted speed for driving
    int driveSpeed = currentMLAdjustedSpeed;
    
    // Get obstacle distances (using existing functions from autonomous_drive)
    float frontDistance = readDistance();  // From autonomous_drive
    float leftDistance = checkLeftDistance();
    float rightDistance = checkRightDistance();
    
    // Note: To use ML predictions, you need to:
    // 1. Implement readSensors() function that returns SensorData
    // 2. Call updateMLPrediction(sensors) periodically
    // For now, we use the last prediction if available
    
    // Obstacle detection threshold - adjust based on ML prediction
    float obstacleThreshold = 20.0;  // cm
    
    // If performance is low, be more cautious (larger threshold)
    if (lastPrediction.success && lastPrediction.level < 4) {
        obstacleThreshold = 30.0;  // More cautious
    } else if (lastPrediction.success && lastPrediction.level > 7) {
        obstacleThreshold = 15.0;  // Can be more aggressive
    }
    
    // Emergency stop if too close
    if (frontDistance < 10.0) {
        emergencyStop();
        driveBackward(driveSpeed / 2);
        delay(500);
        return;
    }
    
    // Obstacle detected ahead
    if (frontDistance < obstacleThreshold) {
        // Check which side is clearer
        if (leftDistance > rightDistance && leftDistance > obstacleThreshold) {
            // Turn left - clearer path
            turnLeft(driveSpeed);
            delay(TURN_DURATION_MS);
        } else if (rightDistance > leftDistance && rightDistance > obstacleThreshold) {
            // Turn right - clearer path
            turnRight(driveSpeed);
            delay(TURN_DURATION_MS);
        } else {
            // Both sides blocked, back up
            driveBackward(driveSpeed / 2);
            delay(500);
            turnRight(driveSpeed);
            delay(TURN_DURATION_MS * 2);
        }
        return;
    }
    
    // No obstacle - drive forward at ML-adjusted speed
    driveForward(driveSpeed);
}

/**
 * Initialize ML-enhanced autonomous driving
 */
void initAutonomousDriveML() {
    // Initialize base autonomous drive
    initAutonomousDrive();
    
    // Check if ML model is available
    if (checkMLModelStatus(ML_SERVER_URL)) {
        Serial.println("✓ ML model is ready - using ML-enhanced driving");
    } else {
        Serial.println("⚠ ML model not available - using base autonomous drive");
    }
    
    // Initialize prediction
    lastPrediction.success = false;
    lastPrediction.level = 5;  // Default to middle level
    lastPrediction.confidence = 0.0;
}

/**
 * Get current ML-adjusted speed
 */
int getMLAdjustedSpeed() {
    return currentMLAdjustedSpeed;
}

/**
 * Get last ML prediction
 */
MLPrediction getLastMLPrediction() {
    return lastPrediction;
}

