# 📊 Data Flow Explanation

## How ESP32 Uses Sensor Data

The ESP32 microcontroller **reads sensors locally** and uses that data in **two ways**:

### 1. **Local Use (TFT Display)**
- ESP32 reads sensors → Calculates performance → Displays on TFT screen
- This happens **on the ESP32 itself** - no WiFi needed!

### 2. **Remote Logging (WiFi to Computer)**
- ESP32 reads sensors → Sends same data via WiFi → Computer saves to CSV
- This is **optional** - car works without WiFi!

---

## Data Flow Diagram

```
┌─────────────────────────────────────────┐
│         ESP32 Microcontroller            │
│                                          │
│  1. Read Sensors (every 200ms)          │
│     ├─ INA219 (current, voltage)        │
│     ├─ MPU6050 (acceleration)           │
│     ├─ HC-SR04 (distance)               │
│     ├─ Encoders (speed)                 │
│     └─ Other sensors...                 │
│                                          │
│  2. Use Data Locally                    │
│     ├─ Calculate performance (Rint)     │
│     └─ Display on TFT screen            │
│                                          │
│  3. Send Data via WiFi (optional)       │
│     └─ HTTP POST to computer            │
│                                          │
└─────────────────────────────────────────┘
         │                    │
         │                    │
    (Local)              (WiFi - Optional)
         │                    │
         ▼                    ▼
┌─────────────────┐  ┌──────────────────┐
│   TFT Display   │  │  Computer Server │
│                 │  │                  │
│ Shows:          │  │ Receives:        │
│ - Performance % │  │ - All sensor data│
│ - Status        │  │ - Saves to CSV   │
└─────────────────┘  └──────────────────┘
```

---

## Code Flow

### Step 1: Read Sensors (Local)
```cpp
SensorData data = readAllSensors();
// This reads:
// - data.current (from INA219)
// - data.voltage (from voltage sensor)
// - data.acceleration (from MPU6050)
// - data.speed (from encoders)
// - etc.
```

### Step 2: Use Data Locally (TFT Display)
```cpp
// Calculate performance using sensor data
float performance = calculatePerformance(
  data.voltage,      // From INA219/voltage sensor
  data.current,      // From INA219
  data.distance,     // From HC-SR04
  data.accelX,       // From MPU6050
  data.accelY,       // From MPU6050
  data.accelZ        // From MPU6050
);

// Display on TFT (happens on ESP32, no WiFi needed!)
updatePerformanceDisplay(performance);
```

### Step 3: Send Data via WiFi (Optional)
```cpp
// Send same sensor data to computer
sendSensorData(data);
// This sends JSON via HTTP POST
// Computer receives and saves to CSV
```

---

## Key Points

✅ **ESP32 reads sensors directly** - sensors are connected to ESP32 GPIO pins  
✅ **TFT display works locally** - no WiFi needed for display  
✅ **WiFi is optional** - car works standalone, WiFi is just for data logging  
✅ **Same data, two uses** - one sensor reading is used for both display and logging  

---

## Sensor Connections

All sensors are **physically connected** to ESP32:

- **INA219** → I2C bus (SDA=21, SCL=22)
- **MPU6050** → I2C bus (SDA=21, SCL=22)
- **HC-SR04** → GPIO 17 (TRIG), GPIO 16 (ECHO)
- **Voltage Sensor** → GPIO 34 (ADC)
- **Encoders** → GPIO 32, 33, 35, 14
- **TFT Display** → SPI (CS=5, DC=4, RST=2, SCK=18, MOSI=23)

---

## Summary

**Question:** How can ESP32 use sensor data if it's sending it via WiFi?

**Answer:** ESP32 reads sensors **locally first**, then:
1. Uses data for TFT display (local, no WiFi)
2. Sends same data via WiFi (optional, for logging)

The sensors are **connected to ESP32**, so ESP32 can read them directly and use the data however it wants - for display, calculations, or sending to computer!

