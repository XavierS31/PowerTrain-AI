/*
 * Autonomous Car with Obstacle Avoidance and WiFi Data Logging
 * 
 * Features:
 * - Three speed levels (Low, Medium, High)
 * - Autonomous driving with obstacle avoidance
 * - HC-SR04 detects obstacles and turns left/right
 * - All sensors read 5 times per second
 * - WiFi communication to send data to computer
 * - TFT display shows Car Driving Behavior Performance (0-100%)
 * - Data saved to software/data/raw/
 * 
 * Two TB6612FNG drivers, each with 2 motors in series on AO1/AO2
 * 
 * PlatformIO Version - Converted from .ino
 */

#include <Arduino.h>
#include "ESP32_car.h"
#include "config/config.h"
#include "sensors/TFT_LED.h"
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <Encoder.h>

// ========== MOTOR CONTROL ==========
#define MOTOR_STBY 27
#define MOTOR_PWMA 25
#define MOTOR_AIN1 26
#define MOTOR_AIN2 13

// Speed Levels (from config.h)
#define SPEED_LOW MOTOR_SPEED_LOW
#define SPEED_MEDIUM MOTOR_SPEED_MEDIUM
#define SPEED_HIGH MOTOR_SPEED_HIGH

#define MIN_MOTOR_SPEED 50
const int PWMFreq = 20000;
const int PWMResolution = 8;
const int motorPWMSpeedChannel = 4;

int currentSpeed = SPEED_MEDIUM;  // Default to medium speed
int currentRightSpeed = 0;
int currentLeftSpeed = 0;

// ========== SENSOR PIN DEFINITIONS ==========
// HC-SR04 Ultrasonic
#define HC_TRIG 17
#define HC_ECHO 16

// DS18B20 Temperature
#define DS18B20_PIN 15

// Voltage Sensor
#define VOLTAGE_SENSE_PIN 34

// I2C Bus
#define I2C_SDA 21
#define I2C_SCL 22

// IR Sensor (TCRT5000)
#define IR_DO 36
#define IR_AO 39

// Encoder pins (Quadrature encoders for speed measurement)
#define ENCODER_LEFT_A 32
#define ENCODER_LEFT_B 33
#define ENCODER_RIGHT_A 35
#define ENCODER_RIGHT_B 14

// ========== OBSTACLE AVOIDANCE (from config.h) ==========
NewPing sonar(HC_TRIG, HC_ECHO, 200);  // Max distance 200cm
#define OBSTACLE_DISTANCE OBSTACLE_DISTANCE_CM
#define TURN_DURATION TURN_DURATION_MS

// ========== SENSOR OBJECTS ==========
OneWire oneWire(DS18B20_PIN);
DallasTemperature tempSensor(&oneWire);
ESP32AnalogRead adc;
Adafruit_INA219 ina219;
Adafruit_MPU6050 mpu;

// Encoder objects for speed measurement
Encoder encoderLeft(ENCODER_LEFT_A, ENCODER_LEFT_B);
Encoder encoderRight(ENCODER_RIGHT_A, ENCODER_RIGHT_B);

// Encoder variables for speed calculation
long lastLeftEncoderCount = 0;
long lastRightEncoderCount = 0;
unsigned long lastSpeedCalcTime = 0;
float currentSpeed_mps = 0.0;  // Speed in meters per second

// Encoder parameters (adjust based on your motor/encoder specs)
#define ENCODER_PULSES_PER_REVOLUTION 20  // Typical value, adjust to your encoder
#define WHEEL_DIAMETER_CM 6.5  // Wheel diameter in cm, adjust to your car
#define WHEEL_CIRCUMFERENCE_CM (3.14159 * WHEEL_DIAMETER_CM)  // cm per revolution

// Sensor initialization flags
bool ina219_initialized = false;
bool mpu_initialized = false;

// ========== WIFI CONFIGURATION (from config.h) ==========
const char* ssid = WIFI_SSID;
const char* password = WIFI_PASSWORD;

// Build server URL from config (will be initialized in setup())
char serverURL[100];

// ========== DATA LOGGING (from config.h) ==========
unsigned long lastSensorRead = 0;
const unsigned long SENSOR_READ_INTERVAL = SENSOR_READ_INTERVAL_MS;

unsigned long lastTFTUpdate = 0;
const unsigned long TFT_UPDATE_INTERVAL = TFT_UPDATE_INTERVAL_MS;

struct SensorData {
  float temperature;
  float voltage;
  float current;
  float power;
  float accelX, accelY, accelZ;
  float gyroX, gyroY, gyroZ;
  float distance;
  float speed;           // Motor speed (PWM value 0-255)
  float acceleration;    // Acceleration magnitude
  int irDigital;
  int irAnalog;
  unsigned long timestamp;
};

// ========== MOTOR CONTROL FUNCTIONS ==========
void applyAcceleration(int targetRightSpeed, int targetLeftSpeed) {
  if (targetRightSpeed > currentRightSpeed) {
    currentRightSpeed += 5;
    if (currentRightSpeed > targetRightSpeed) currentRightSpeed = targetRightSpeed;
  } else if (targetRightSpeed < currentRightSpeed) {
    currentRightSpeed -= 8;
    if (currentRightSpeed < targetRightSpeed) currentRightSpeed = targetRightSpeed;
  }
  
  if (targetLeftSpeed > currentLeftSpeed) {
    currentLeftSpeed += 5;
    if (currentLeftSpeed > targetLeftSpeed) currentLeftSpeed = targetLeftSpeed;
  } else if (targetLeftSpeed < currentLeftSpeed) {
    currentLeftSpeed -= 8;
    if (currentLeftSpeed < targetLeftSpeed) currentLeftSpeed = targetLeftSpeed;
  }
}

void rotateMotor(int rightMotorSpeed, int leftMotorSpeed) {
  applyAcceleration(rightMotorSpeed, leftMotorSpeed);
  
  int combinedSpeed = (currentRightSpeed + currentLeftSpeed) / 2;
  
  // Ensure STBY is HIGH (motors enabled)
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
  
  // Write PWM value
  ledcWrite(motorPWMSpeedChannel, pwmValue);
  
  // Debug output (only occasionally to avoid spam)
  static unsigned long lastMotorDebug = 0;
  if (millis() - lastMotorDebug > 1000) {
    Serial.print("Motor PWM: ");
    Serial.print(pwmValue);
    Serial.print("/255, AIN1: ");
    Serial.print(digitalRead(MOTOR_AIN1));
    Serial.print(", AIN2: ");
    Serial.print(digitalRead(MOTOR_AIN2));
    Serial.print(", STBY: ");
    Serial.println(digitalRead(MOTOR_STBY));
    lastMotorDebug = millis();
  }
}

void stopMotors() {
  rotateMotor(0, 0);
}

void driveForward(int speed) {
  Serial.print("driveForward called with speed: ");
  Serial.println(speed);
  rotateMotor(speed, speed);
  Serial.print("Motor speeds - Right: ");
  Serial.print(currentRightSpeed);
  Serial.print(", Left: ");
  Serial.println(currentLeftSpeed);
}

void turnLeft(int speed) {
  rotateMotor(speed, -speed);  // Right forward, left reverse
}

void turnRight(int speed) {
  rotateMotor(-speed, speed);  // Right reverse, left forward
}

// ========== SENSOR READING FUNCTIONS ==========
SensorData readAllSensors() {
  SensorData data;
  data.timestamp = millis();
  
  // Temperature (DS18B20)
  tempSensor.requestTemperatures();
  data.temperature = tempSensor.getTempCByIndex(0);
  
  // Voltage
  float adcValue = adc.readVoltage();
  data.voltage = adcValue * 5.0;  // Adjust multiplier based on voltage divider ratio
  
  // Current and Power (INA219)
  // Always try to read, even if initialization reported failure
  // Sometimes the sensor works but initialization check fails
  float current_mA = ina219.getCurrent_mA();
  float power_mW = ina219.getPower_mW();
  
  if (ina219_initialized || (current_mA != 0 || power_mW != 0)) {
    data.current = current_mA / 1000.0;  // Convert to Amps
    data.power = power_mW / 1000.0;       // Convert to Watts
    // If we got a reading, mark as initialized for future
    if (!ina219_initialized && (current_mA != 0 || power_mW != 0)) {
      ina219_initialized = true;
      Serial.println("INA219 working (late initialization)");
    }
  } else {
    data.current = 0;
    data.power = 0;
  }
  
  // Calculate speed from encoder readings (actual speed in m/s)
  unsigned long currentTime = millis();
  if (lastSpeedCalcTime > 0 && (currentTime - lastSpeedCalcTime) >= 100) {  // Calculate every 100ms
    long leftCount = encoderLeft.read();
    long rightCount = encoderRight.read();
    
    // Calculate encoder change (counts)
    long leftDelta = abs(leftCount - lastLeftEncoderCount);
    long rightDelta = abs(rightCount - lastRightEncoderCount);
    long avgDelta = (leftDelta + rightDelta) / 2;
    
    // Calculate time delta (seconds)
    float timeDelta = (currentTime - lastSpeedCalcTime) / 1000.0;
    
    // Calculate speed: (encoder_counts / pulses_per_rev) * (wheel_circumference / time)
    // Convert to meters per second
    float revolutions = (float)avgDelta / ENCODER_PULSES_PER_REVOLUTION;
    float distance_cm = revolutions * WHEEL_CIRCUMFERENCE_CM;
    currentSpeed_mps = (distance_cm / 100.0) / timeDelta;  // Convert cm to m, divide by time
    
    // Update last values
    lastLeftEncoderCount = leftCount;
    lastRightEncoderCount = rightCount;
    lastSpeedCalcTime = currentTime;
  } else if (lastSpeedCalcTime == 0) {
    // First reading - initialize
    lastLeftEncoderCount = encoderLeft.read();
    lastRightEncoderCount = encoderRight.read();
    lastSpeedCalcTime = currentTime;
    currentSpeed_mps = 0.0;
  }
  
  // Store speed in m/s (can also convert to km/h or mph if needed)
  data.speed = currentSpeed_mps;  // Speed in meters per second
  
  // Accelerometer and Gyroscope (MPU6050)
  if (mpu_initialized) {
    sensors_event_t accel, gyro, temp;
    if (mpu.getEvent(&accel, &gyro, &temp)) {
      data.accelX = accel.acceleration.x;
      data.accelY = accel.acceleration.y;
      data.accelZ = accel.acceleration.z;
      data.gyroX = gyro.gyro.x;
      data.gyroY = gyro.gyro.y;
      data.gyroZ = gyro.gyro.z;
      
      // Calculate acceleration magnitude
      data.acceleration = sqrt(accel.acceleration.x * accel.acceleration.x + 
                                accel.acceleration.y * accel.acceleration.y + 
                                accel.acceleration.z * accel.acceleration.z);
    } else {
      data.accelX = data.accelY = data.accelZ = 0;
      data.gyroX = data.gyroY = data.gyroZ = 0;
      data.acceleration = 0;
    }
  } else {
    data.accelX = data.accelY = data.accelZ = 0;
    data.gyroX = data.gyroY = data.gyroZ = 0;
    data.acceleration = 0;
  }
  
  // Distance (HC-SR04)
  data.distance = sonar.ping_cm();
  if (data.distance == 0) data.distance = 200;  // Max range if no echo
  
  // IR Sensor
  data.irDigital = digitalRead(IR_DO);
  data.irAnalog = analogRead(IR_AO);
  
  return data;
}

// ========== WIFI DATA TRANSMISSION ==========
void sendSensorData(SensorData data) {
  // Try to send data, but don't block if WiFi is not connected
  // Car should work standalone without WiFi
  if (WiFi.status() != WL_CONNECTED) {
    // Optionally try to reconnect WiFi (non-blocking)
    static unsigned long lastWiFiReconnect = 0;
    if (millis() - lastWiFiReconnect > 10000) {  // Try every 10 seconds
      WiFi.disconnect();
      WiFi.begin(ssid, password);
      lastWiFiReconnect = millis();
    }
    return;  // Continue operation without WiFi
  }
  
  HTTPClient http;
  http.begin(serverURL);
  http.addHeader("Content-Type", "application/json");
  
  // Create JSON
  JsonDocument doc;
  doc["timestamp"] = data.timestamp;
  doc["temperature"] = data.temperature;
  doc["voltage"] = data.voltage;
  doc["current"] = data.current;
  doc["power"] = data.power;
  doc["accelX"] = data.accelX;
  doc["accelY"] = data.accelY;
  doc["accelZ"] = data.accelZ;
  doc["acceleration"] = data.acceleration;  // Acceleration magnitude
  doc["gyroX"] = data.gyroX;
  doc["gyroY"] = data.gyroY;
  doc["gyroZ"] = data.gyroZ;
  doc["distance"] = data.distance;
  doc["speed"] = data.speed;  // Speed in m/s from encoders
  doc["acceleration"] = data.acceleration;  // Acceleration magnitude from MPU6050
  doc["irDigital"] = data.irDigital;
  doc["irAnalog"] = data.irAnalog;
  
  String jsonString;
  serializeJson(doc, jsonString);
  
  int httpResponseCode = http.POST(jsonString);
  
  if (httpResponseCode > 0) {
    Serial.print("Data sent: ");
    Serial.println(httpResponseCode);
  } else {
    Serial.print("Error sending data: ");
    Serial.println(httpResponseCode);
  }
  
  http.end();
}

// ========== OBSTACLE AVOIDANCE LOGIC ==========
void autonomousDrive() {
  float distance = sonar.ping_cm();
  if (distance == 0) distance = 200;  // Max range
  
  if (distance < OBSTACLE_DISTANCE) {
    // Obstacle detected - turn away
    Serial.print("Obstacle detected at ");
    Serial.print(distance);
    Serial.println(" cm");
    
    // Check left and right distances (simplified - just turn right)
    // In a more advanced version, you could use multiple sensors
    stopMotors();
    delay(100);
    
    // Turn right
    turnRight(currentSpeed);
    delay(TURN_DURATION);
    stopMotors();
    delay(100);
    
    // Check if clear
    distance = sonar.ping_cm();
    if (distance < OBSTACLE_DISTANCE) {
      // Still blocked, try left
      turnLeft(currentSpeed);
      delay(TURN_DURATION);
      stopMotors();
    }
  } else {
    // Clear path - drive forward
    driveForward(currentSpeed);
  }
}

// ========== SETUP ==========
void setup() {
  Serial.begin(115200);
  
  // Initialize TFT Display
  initTFT();
  // Display hardcoded static values immediately (no sensor dependency)
  // 50% Fair, 0.7 ohm RINT, 6.0V (4-pack AA), 30°C temp
  displayPerformance(50.0, 0.7, 6.0, 30.0);  // 50% Fair, hardcoded values
  
  // Initialize motor pins
  pinMode(MOTOR_AIN1, OUTPUT);
  pinMode(MOTOR_AIN2, OUTPUT);
  pinMode(MOTOR_PWMA, OUTPUT);
  pinMode(MOTOR_STBY, OUTPUT);
  digitalWrite(MOTOR_STBY, HIGH);
  digitalWrite(MOTOR_AIN1, LOW);
  digitalWrite(MOTOR_AIN2, LOW);
  
  ledcSetup(motorPWMSpeedChannel, PWMFreq, PWMResolution);
  ledcAttachPin(MOTOR_PWMA, motorPWMSpeedChannel);
  
  // Initialize I2C
  Wire.begin(I2C_SDA, I2C_SCL);
  
  // Initialize sensors
  tempSensor.begin();
  adc.attach(VOLTAGE_SENSE_PIN);
  
  // Initialize INA219
  // INA219 has fixed I2C address (0x40), but begin() doesn't take address parameter
  // The library automatically uses the default address
  if (ina219.begin()) {
    ina219_initialized = true;
    Serial.println("INA219 initialized");
  } else {
    Serial.println("INA219 not found! Check I2C connections.");
    Serial.println("INA219 default address is 0x40");
    ina219_initialized = false;
  }
  
  // Initialize MPU6050
  if (mpu.begin()) {
    mpu_initialized = true;
    mpu.setAccelerometerRange(MPU6050_RANGE_2_G);
    mpu.setGyroRange(MPU6050_RANGE_250_DEG);
    Serial.println("MPU6050 initialized");
  } else {
    Serial.println("MPU6050 not found!");
    mpu_initialized = false;
  }
  
  // Initialize IR sensor pins
  pinMode(IR_DO, INPUT);
  pinMode(IR_AO, INPUT);
  
  // Initialize encoder pins (Encoder library handles pin setup)
  // Encoders are interrupt-driven, no manual pinMode needed
  encoderLeft.write(0);  // Reset encoder counts
  encoderRight.write(0);
  lastLeftEncoderCount = 0;
  lastRightEncoderCount = 0;
  lastSpeedCalcTime = 0;
  
  // Connect to WiFi
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 20) {
    delay(500);
    Serial.print(".");
    attempts++;
  }
  
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println();
    Serial.print("Connected! IP: ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println();
    Serial.println("WiFi connection failed - continuing without WiFi");
  }
  
  // Build server URL from config
  snprintf(serverURL, sizeof(serverURL), "http://%s:%d/api/data", SERVER_IP, SERVER_PORT);
  Serial.print("Server URL: ");
  Serial.println(serverURL);
  
  Serial.println("Autonomous car ready!");
  Serial.println("Speed: MEDIUM (change with Serial commands: L/M/H)");
  
  // Start driving immediately (autonomous mode)
  Serial.println("Starting autonomous driving...");
  
  // Ensure motor control pins are properly set
  digitalWrite(MOTOR_STBY, HIGH);  // Enable motors
  delay(100);  // Small delay to ensure pin is set
  
  Serial.print("Motor STBY pin state: "); Serial.println(digitalRead(MOTOR_STBY));
  Serial.print("Current speed setting: "); Serial.println(currentSpeed);
  
  // Initialize motor speeds to 0
  currentRightSpeed = 0;
  currentLeftSpeed = 0;
  
  // Force immediate motor start (bypass acceleration for initial start)
  digitalWrite(MOTOR_AIN1, HIGH);
  digitalWrite(MOTOR_AIN2, LOW);
  int pwmValue = currentSpeed;
  if (pwmValue > 0 && pwmValue < MIN_MOTOR_SPEED) {
    pwmValue = MIN_MOTOR_SPEED;
  }
  ledcWrite(motorPWMSpeedChannel, pwmValue);
  currentRightSpeed = currentSpeed;
  currentLeftSpeed = currentSpeed;
  
  delay(200);  // Give motors time to start
  
  Serial.println("Motors started!");
  Serial.print("Motor PWM value: "); Serial.println(pwmValue);
  Serial.print("Motor speeds - Right: ");
  Serial.print(currentRightSpeed);
  Serial.print(", Left: ");
  Serial.println(currentLeftSpeed);
  Serial.println("Car should now be driving forward!");
}

// ========== MAIN LOOP ==========
void loop() {
  // Check for speed change via Serial (non-blocking)
  if (Serial.available()) {
    char cmd = Serial.read();
    if (cmd == 'L' || cmd == 'l') {
      currentSpeed = SPEED_LOW;
      Serial.println("Speed: LOW");
    } else if (cmd == 'M' || cmd == 'm') {
      currentSpeed = SPEED_MEDIUM;
      Serial.println("Speed: MEDIUM");
    } else if (cmd == 'H' || cmd == 'h') {
      currentSpeed = SPEED_HIGH;
      Serial.println("Speed: HIGH");
    } else if (cmd == 'S' || cmd == 's') {
      stopMotors();
      Serial.println("Stopped");
    }
    // Clear any remaining serial buffer
    while (Serial.available()) Serial.read();
  }
  
  // Autonomous driving with obstacle avoidance (runs continuously)
  // Run autonomous drive every 100ms (10 times per second) - frequent enough for smooth control
  static unsigned long lastAutonomousCall = 0;
  if (millis() - lastAutonomousCall >= 100) {  // Run every 100ms
    autonomousDrive();
    lastAutonomousCall = millis();
  }
  
  // Read and send sensor data (5 times per second)
  if (millis() - lastSensorRead >= SENSOR_READ_INTERVAL) {
    SensorData data = readAllSensors();
    sendSensorData(data);
    
  // Update TFT display with hardcoded values (every 2 seconds to avoid flickering)
  // Using hardcoded static values - no sensor data dependency
  if (millis() - lastTFTUpdate >= 2000) {  // Update every 2 seconds
    // Hardcoded values: 50% performance, 0.7 ohm RINT, 6.0V (4-pack AA), 30°C temp
    displayPerformance(50.0, 0.7, 6.0, 30.0);  // Hardcoded static values
    lastTFTUpdate = millis();
  }
    
    // Also print to Serial for debugging
    Serial.print("Temp: ");
    Serial.print(data.temperature);
    Serial.print("°C, Voltage: ");
    Serial.print(data.voltage);
    Serial.print("V, Current: ");
    Serial.print(data.current);
    Serial.print("A, Speed: ");
    Serial.print(data.speed);
    Serial.print(" m/s, Accel: ");
    Serial.print(data.acceleration);
    Serial.print(" m/s², Distance: ");
    Serial.print(data.distance);
    Serial.print(" cm, Performance: ");
    Serial.print(performance);
    Serial.println("%");
    
    lastSensorRead = millis();
  }
  
  delay(10);
}

