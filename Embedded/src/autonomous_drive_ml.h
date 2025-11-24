#ifndef AUTONOMOUS_DRIVE_ML_H
#define AUTONOMOUS_DRIVE_ML_H

#include "autonomous_drive.h"
#include "ml_predictor.h"

/**
 * ML-Enhanced Autonomous Driving
 * 
 * This module integrates ML predictions into autonomous driving.
 * The car adjusts speed and behavior based on predicted performance level.
 */

// ========== INITIALIZATION ==========
void initAutonomousDriveML();

// ========== MAIN ML-ENHANCED DRIVING FUNCTION ==========
void autonomousDriveML();

// ========== ML PREDICTION UPDATES ==========
void updateMLPrediction(SensorData sensorData);

// ========== STATUS ==========
int getMLAdjustedSpeed();
MLPrediction getLastMLPrediction();

#endif // AUTONOMOUS_DRIVE_ML_H

