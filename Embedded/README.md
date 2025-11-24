# PowerTrain AI - Embedded Module

ESP32-based autonomous car with sensor integration and ML-powered performance prediction.

## Project Structure

```
Embedded/
├── src/
│   ├── main.cpp                    # Main program entry point
│   ├── ESP32_car.h                # Core library includes
│   ├── config/
│   │   └── config.h               # Configuration constants
│   ├── sensors/
│   │   ├── DS18B20_temp.cpp       # Temperature sensor
│   │   ├── HC-SRO4_Infrared.cpp   # Ultrasonic distance sensor
│   │   ├── I2C_BUS.cpp            # I2C bus initialization
│   │   ├── IR_MH_Sensor.cpp       # Infrared obstacle sensor
│   │   ├── TB6612FNG_MotorStepper.cpp  # Motor driver
│   │   ├── TFT_LED.cpp            # TFT display functions
│   │   ├── TFT_LED.h             # TFT display header
│   │   └── voltage_sensor.cpp    # Voltage detection
│   ├── autonomous_drive.h         # Autonomous driving header
│   ├── autonomous_drive_ml.h      # ML-enhanced driving header
│   ├── autonomous_drive_ml.cpp    # ML-enhanced driving implementation
│   ├── ml_predictor.h             # ML prediction API header
│   └── ml_predictor.cpp           # ML prediction API implementation
│
├── autonomous_Driving/
│   └── autonomous_Driving.ino     # Standalone autonomous driving sketch
│
├── bluetooth_Driving/
│   └── bluetooth_Driving.ino      # Bluetooth control sketch
│
├── compile_and_upload_autonomous.bat  # Build and upload script
└── platformio.ini                 # PlatformIO configuration
```

## Core Files

### Main Entry Point
- `src/main.cpp` - Main program loop, initializes sensors and runs autonomous driving

### Configuration
- `src/config/config.h` - Pin definitions, thresholds, timing constants

### Sensor Drivers
- `src/sensors/DS18B20_temp.cpp` - DS18B20 temperature sensor driver
- `src/sensors/HC-SRO4_Infrared.cpp` - HC-SR04 ultrasonic distance sensor
- `src/sensors/IR_MH_Sensor.cpp` - Infrared obstacle detection
- `src/sensors/voltage_sensor.cpp` - Battery voltage monitoring
- `src/sensors/TB6612FNG_MotorStepper.cpp` - Motor control driver
- `src/sensors/TFT_LED.cpp` - ST7735S TFT display driver
- `src/sensors/I2C_BUS.cpp` - I2C bus setup for sensors

### Autonomous Driving
- `src/autonomous_drive.h` - Basic autonomous driving interface
- `src/autonomous_drive_ml.h` - ML-enhanced autonomous driving interface
- `src/autonomous_drive_ml.cpp` - ML-enhanced driving with performance-based adjustments

### Machine Learning Integration
- `src/ml_predictor.h` - ML prediction API interface
- `src/ml_predictor.cpp` - HTTP client for Python ML server predictions

### Library Includes
- `src/ESP32_car.h` - Central header with all required library includes

## Compilation and Upload

### Using Batch Script

```bash
cd Embedded
compile_and_upload_autonomous.bat
```

This script:
1. Copies autonomous code to main.cpp
2. Compiles with PlatformIO
3. Prompts for ESP32 upload

### Using PlatformIO Directly

```bash
cd Embedded
pio run -t upload
```

## Hardware Configuration

### Sensors
- Temperature: DS18B20 (OneWire)
- Voltage: 0-25V detection module (GPIO 34)
- Current/Power: INA219 (I2C)
- Accelerometer/Gyro: MPU6050 (I2C)
- Distance: HC-SR04 (GPIO 17 trigger, GPIO 16 echo)
- Infrared: TCRT5000 (GPIO 36 digital, GPIO 39 analog)

### Actuators
- Motors: TB6612FNG driver (PWM control)
- Display: ST7735S TFT (SPI)

### Communication
- WiFi: For data transmission and ML predictions
- Serial: Debug output (115200 baud)

## Autonomous Driving Modes

### Basic Autonomous Mode
Uses obstacle avoidance with ultrasonic and IR sensors:
- Forward movement with obstacle detection
- Turn left/right based on clearer path
- Backup and turn when blocked

### ML-Enhanced Autonomous Mode
Integrates ML predictions to adjust behavior:
- Speed adjustment based on predicted performance level
- Obstacle threshold adaptation
- Confidence-based decision making

## ML Prediction Integration

The car can request performance predictions from the Python server:

```cpp
#include "src/ml_predictor.h"

SensorData data = readSensors();
MLPrediction pred = getMLPrediction(data, "http://SERVER_IP:5000");

if (pred.success) {
    // Use pred.level (1-10) and pred.confidence (0.0-1.0)
    adjustBehavior(pred);
}
```

## Key Functions

### Sensor Reading
- `readSensors()` - Read all sensor values
- `readDistance()` - Get ultrasonic distance
- `readIRSensor()` - Check IR obstacle detection
- `checkLeftDistance()` - Scan left side
- `checkRightDistance()` - Scan right side

### Motor Control
- `driveForward(speed)` - Move forward
- `turnLeft(speed)` - Turn left
- `turnRight(speed)` - Turn right
- `driveBackward(speed)` - Reverse
- `stopMotors()` - Stop all motors
- `emergencyStop()` - Immediate stop

### Autonomous Driving
- `autonomousDrive()` - Basic autonomous mode
- `autonomousDriveML()` - ML-enhanced autonomous mode
- `initAutonomousDrive()` - Initialize autonomous system
- `initAutonomousDriveML()` - Initialize ML-enhanced system

### ML Predictions
- `getMLPrediction(sensorData, serverURL)` - Get prediction from server
- `checkMLModelStatus(serverURL)` - Check if ML model is loaded

## Configuration

Edit `src/config/config.h` to adjust:
- Pin assignments
- Motor speeds
- Obstacle detection thresholds
- Sensor calibration values
- WiFi credentials
- Server URL

## Dependencies

PlatformIO libraries (defined in platformio.ini):
- OneWire
- DallasTemperature
- Adafruit INA219
- Adafruit MPU6050
- NewPing
- Adafruit ST7735
- Adafruit GFX
- Encoder
- ESP32AnalogRead
- ArduinoJson
- HTTPClient

## Development Workflow

1. Edit source files in `src/`
2. Update configuration in `src/config/config.h`
3. Compile: `pio run`
4. Upload: `pio run -t upload`
5. Monitor: `pio device monitor`

## Troubleshooting

- Upload fails: Put ESP32 in download mode (hold BOOT, press RESET, release BOOT)
- WiFi not connecting: Check credentials in config.h
- Sensors not reading: Verify I2C connections and addresses
- Motors not moving: Check motor driver connections and PWM pins
- ML predictions fail: Ensure Python server is running and accessible

