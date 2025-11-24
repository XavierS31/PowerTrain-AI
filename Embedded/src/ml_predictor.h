#ifndef ML_PREDICTOR_H
#define ML_PREDICTOR_H

#include <Arduino.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <WiFi.h>

// Structure to hold prediction result
struct MLPrediction {
    int level;              // Predicted performance level (1-10)
    float confidence;       // Confidence score (0.0-1.0)
    bool success;           // Whether prediction was successful
    String error_message;  // Error message if failed
};

/**
 * Get ML model prediction from Python server
 * 
 * @param sensorData Sensor data structure with voltage, current, temperature, etc.
 * @param serverURL Base URL of the server (e.g., "http://192.168.1.100:5000")
 * @return MLPrediction structure with prediction results
 * 
 * Example usage:
 *   SensorData data = readSensors();
 *   MLPrediction pred = getMLPrediction(data, "http://192.168.1.100:5000");
 *   if (pred.success) {
 *     Serial.print("Predicted Level: ");
 *     Serial.println(pred.level);
 *     Serial.print("Confidence: ");
 *     Serial.println(pred.confidence);
 *   }
 */
MLPrediction getMLPrediction(SensorData sensorData, const char* serverURL);

/**
 * Check if ML model is loaded on the server
 * 
 * @param serverURL Base URL of the server
 * @return true if model is loaded, false otherwise
 */
bool checkMLModelStatus(const char* serverURL);

#endif // ML_PREDICTOR_H

