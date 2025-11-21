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
 */

#include "src/ESP32_car.h"
#include "src/config/config.h"
#include "src/sensors/TFT_LED.h"
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

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

// Sensor initialization flags
bool ina219_initialized = false;
bool mpu_initialized = false;

// ========== WIFI CONFIGURATION (from config.h) ==========
const char* ssid = WIFI_SSID;
const char* password = WIFI_PASSWORD;

// Build server URL from config
char serverURL[100];
sprintf(serverURL, "http://%s:%d/api/data", SERVER_IP, SERVER_PORT);

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
  float adcValue = adc.readVoltage(VOLTAGE_SENSE_PIN);
  data.voltage = adcValue * 5.0;  // Adjust multiplier based on voltage divider ratio
  
  // Current and Power (INA219)
  if (ina219_initialized) {
    data.current = ina219.getCurrent_mA() / 1000.0;  // Convert to Amps
    data.power = ina219.getPower_mW() / 1000.0;       // Convert to Watts
  } else {
    data.current = 0;
    data.power = 0;
  }
  
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
    } else {
      data.accelX = data.accelY = data.accelZ = 0;
      data.gyroX = data.gyroY = data.gyroZ = 0;
    }
  } else {
    data.accelX = data.accelY = data.accelZ = 0;
    data.gyroX = data.gyroY = data.gyroZ = 0;
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
  if (WiFi.status() != WL_CONNECTED) {
    return;
  }
  
  HTTPClient http;
  http.begin(serverURL);
  http.addHeader("Content-Type", "application/json");
  
  // Create JSON
  StaticJsonDocument<512> doc;
  doc["timestamp"] = data.timestamp;
  doc["temperature"] = data.temperature;
  doc["voltage"] = data.voltage;
  doc["current"] = data.current;
  doc["power"] = data.power;
  doc["accelX"] = data.accelX;
  doc["accelY"] = data.accelY;
  doc["accelZ"] = data.accelZ;
  doc["gyroX"] = data.gyroX;
  doc["gyroY"] = data.gyroY;
  doc["gyroZ"] = data.gyroZ;
  doc["distance"] = data.distance;
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
  displayPerformance(0);  // Start at 0%
  
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
  if (ina219.begin()) {
    ina219_initialized = true;
    Serial.println("INA219 initialized");
  } else {
    Serial.println("INA219 not found!");
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
  
  Serial.println("Autonomous car ready!");
  Serial.println("Speed: MEDIUM (change with Serial commands: L/M/H)");
}

// ========== MAIN LOOP ==========
void loop() {
  // Check for speed change via Serial
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
  }
  
  // Autonomous driving with obstacle avoidance
  autonomousDrive();
  
  // Read and send sensor data (5 times per second)
  if (millis() - lastSensorRead >= SENSOR_READ_INTERVAL) {
    SensorData data = readAllSensors();
    sendSensorData(data);
    
    // Calculate and update TFT performance display
    float performance = calculatePerformance(
      data.voltage, 
      data.current, 
      data.distance, 
      data.accelX, 
      data.accelY, 
      data.accelZ
    );
    
    // Update TFT display (every 500ms to avoid flickering)
    if (millis() - lastTFTUpdate >= TFT_UPDATE_INTERVAL) {
      updatePerformanceDisplay(performance);
      lastTFTUpdate = millis();
    }
    
    // Also print to Serial for debugging
    Serial.print("Temp: ");
    Serial.print(data.temperature);
    Serial.print("°C, Voltage: ");
    Serial.print(data.voltage);
    Serial.print("V, Distance: ");
    Serial.print(data.distance);
    Serial.print(" cm, Performance: ");
    Serial.print(performance);
    Serial.println("%");
    
    lastSensorRead = millis();
  }
  
  delay(10);
}

