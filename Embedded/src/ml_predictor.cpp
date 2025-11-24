#include "ml_predictor.h"
#include "ESP32_car.h"  // For SensorData structure

MLPrediction getMLPrediction(SensorData sensorData, const char* serverURL) {
    MLPrediction result;
    result.success = false;
    result.level = 0;
    result.confidence = 0.0;
    result.error_message = "";
    
    // Check WiFi connection
    if (WiFi.status() != WL_CONNECTED) {
        result.error_message = "WiFi not connected";
        return result;
    }
    
    // Construct prediction endpoint URL
    String predictURL = String(serverURL) + "/api/predict";
    
    HTTPClient http;
    http.begin(predictURL);
    http.addHeader("Content-Type", "application/json");
    http.setTimeout(5000);  // 5 second timeout
    
    // Create JSON payload with sensor data
    StaticJsonDocument<512> doc;
    doc["voltage"] = sensorData.voltage;
    doc["current"] = sensorData.current;
    doc["temperature"] = sensorData.temperature;
    doc["accelX"] = sensorData.accelX;
    doc["accelY"] = sensorData.accelY;
    doc["accelZ"] = sensorData.accelZ;
    // Note: speed may not be in SensorData struct - calculate from encoders or use 0.0
    doc["speed"] = 0.0;  // TODO: Calculate from encoders if available
    doc["distance"] = sensorData.distance;
    doc["timestamp"] = sensorData.timestamp;
    
    String jsonString;
    serializeJson(doc, jsonString);
    
    // Send POST request
    int httpResponseCode = http.POST(jsonString);
    
    if (httpResponseCode == 200) {
        // Parse response
        String response = http.getString();
        StaticJsonDocument<256> responseDoc;
        DeserializationError error = deserializeJson(responseDoc, response);
        
        if (!error && responseDoc["status"] == "success") {
            result.success = true;
            result.level = responseDoc["predicted_level"];
            result.confidence = responseDoc["confidence"];
        } else {
            result.error_message = "Failed to parse response";
        }
    } else if (httpResponseCode == 503) {
        result.error_message = "Model not loaded on server";
    } else {
        result.error_message = "HTTP error: " + String(httpResponseCode);
    }
    
    http.end();
    return result;
}

bool checkMLModelStatus(const char* serverURL) {
    if (WiFi.status() != WL_CONNECTED) {
        return false;
    }
    
    String statusURL = String(serverURL) + "/api/status";
    
    HTTPClient http;
    http.begin(statusURL);
    http.setTimeout(3000);
    
    int httpResponseCode = http.GET();
    bool modelLoaded = false;
    
    if (httpResponseCode == 200) {
        String response = http.getString();
        StaticJsonDocument<256> doc;
        DeserializationError error = deserializeJson(doc, response);
        
        if (!error) {
            modelLoaded = doc["model_loaded"] | false;
        }
    }
    
    http.end();
    return modelLoaded;
}

