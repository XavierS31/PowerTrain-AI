# ⚡ PowerTrain AI

PowerTrain AI is a comprehensive autonomous car system powered by ESP32, featuring real-time sensor data collection, machine learning-based performance prediction, and intelligent autonomous driving capabilities. The system integrates embedded hardware, Python-based ML models, and RESTful APIs to create a complete end-to-end solution for autonomous vehicle control and battery health monitoring.

## 🚀 Overview

PowerTrain AI combines embedded systems engineering with machine learning to create an intelligent autonomous car that adapts its driving behavior based on real-time performance predictions. The system collects sensor data from multiple sources (temperature, voltage, current, acceleration, distance), processes it through trained ML models, and uses predictions to optimize autonomous driving decisions in real-time.

The project consists of three main components:
- **Embedded System**: ESP32-based car with sensor integration and autonomous driving logic
- **Machine Learning**: Three ML models (PyTorch NN, scikit-learn MLP, Random Forest) for performance prediction
- **Data Infrastructure**: Flask servers for data collection, ML predictions, and database management

## ⚙️ Features

### 🚗 Autonomous Driving

- **Basic Autonomous Mode**: Obstacle avoidance using ultrasonic (HC-SR04) and infrared sensors
- **ML-Enhanced Autonomous Mode**: Performance-based speed and behavior adjustment using real-time ML predictions
- **Three Speed Levels**: Low (85 PWM), Medium (170 PWM), High (255 PWM) with smooth acceleration/deceleration
- **Intelligent Navigation**: Left/right path selection based on obstacle detection and distance scanning
- **Emergency Stop**: Immediate stop on critical obstacle detection

### 📊 Sensor Data Collection

- **Temperature Monitoring**: DS18B20 digital temperature sensor (OneWire protocol)
- **Voltage Detection**: 0-25V voltage sensor module (GPIO 34 ADC)
- **Current/Power Measurement**: INA219 I2C sensor for precise current and power readings
- **Accelerometer/Gyroscope**: MPU6050 6-axis IMU (I2C) for acceleration and orientation
- **Distance Sensing**: HC-SR04 ultrasonic sensor (GPIO 17/16) for obstacle detection
- **Infrared Obstacle Detection**: TCRT5000 IR sensor (GPIO 36/39) for close-range detection
- **Speed Measurement**: Encoder-based speed calculation from motor rotations

### 🧠 Machine Learning Models

#### PyTorch Neural Network
- **Architecture**: 2 hidden layers with 16 neurons each
- **Activation**: ReLU with Dropout (0.2) for regularization
- **Framework**: PyTorch
- **Output**: Performance level classification (1-10)
- **Saved as**: `pytorch_nn_model.pth`

#### scikit-learn MLP (Multi-Layer Perceptron)
- **Architecture**: 2 hidden layers (16, 8 neurons)
- **Framework**: scikit-learn
- **Solver**: Adam optimizer
- **Output**: Performance level classification (1-10)
- **Saved as**: `mlp_model.joblib`

#### Random Forest Classifier
- **Parameters**: 100 decision trees, max depth 20
- **Framework**: scikit-learn
- **Features**: Feature importance analysis
- **Output**: Performance level classification (1-10)
- **Saved as**: `random_forest_model.joblib`

### 📡 Data Infrastructure

- **Data Receiver API**: Flask server receiving sensor data from ESP32 via WiFi
- **ML Prediction API**: Real-time performance predictions from trained models
- **Data Query API**: RESTful API for querying SQLite database
- **Multiple Data Formats**: JSON, CSV, SQLite, Parquet, HDF5, Excel support
- **SQLite Database**: Structured storage for driving behavior data with indexes

### 🖥️ TFT Display

- **ST7735S Display**: 1.8" TFT screen showing real-time performance metrics
- **Performance Percentage**: Calculated from internal resistance, temperature, and current
- **Visual Feedback**: Color-coded performance indicators

## 🧑‍💻 Tech Stack

| Layer | Technologies |
|-------|-------------|
| **Embedded** | ESP32 DevKit V1, Arduino Framework, PlatformIO, C++, C|
| **Sensors** | DS18B20, INA219, MPU6050, HC-SR04, TCRT5000, Voltage Sensor |
| **Actuators** | TB6612FNG Motor Driver, ST7735S TFT Display |
| **Communication** | WiFi (HTTP), Bluetooth (optional), Serial (115200 baud) |
| **Backend** | Python, Flask, SQLite |
| **Machine Learning** | PyTorch, scikit-learn, NumPy, Pandas, Matplotlib |
| **Data Processing** | Pandas, NumPy, StandardScaler |
| **Visualization** | Matplotlib, Seaborn |
| **Development** | PlatformIO, Arduino IDE compatible |

## 🛠️ Setup Instructions

### Prerequisites

- **Python** (for ML models and data servers)
- **Node.js** (optional, for some utilities)
- **PlatformIO** or **Arduino IDE** (for ESP32 development)
- **ESP32 DevKit V1** connected via USB
- **Git**

### 1. Clone the Repository

```bash
git clone https://github.com/XavierS31/PowerTrain-AI.git
cd PowerTrain-AI
```

### 2. Python Environment Setup

```bash
# Install Python dependencies
cd Software
pip install -r requirements.txt
```

Required packages:
- `pandas`, `numpy`, `scikit-learn`, `torch` (PyTorch)
- `flask`, `flask-cors`, `python-dotenv`
- `matplotlib`, `seaborn`, `joblib`
- `sqlite3` (built-in)

### 3. Database Setup

```bash
cd Software/src
python create_database.py
```

This creates:
- SQLite database at `Software/data/database/driving_behavior.db`
- Schema with indexes for performance
- Imports data from `driving_behavior_dataset.csv` (if available)

### 4. Train Machine Learning Models

Train all three models at once:

```bash
cd Software/src
python save_model.py
```

Or train individually:

```bash
# PyTorch Neural Network
python train_pytorch_nn.py

# scikit-learn MLP
python train_mlp.py

# Random Forest
python train_random_forest.py
```

Models are saved to `Software/models/`:
- `pytorch_nn_model.pth` + `pytorch_scaler.joblib`
- `mlp_model.joblib` + `scaler.joblib`
- `random_forest_model.joblib` + `random_forest_scaler.joblib`

### 5. Evaluate Models

Compare all trained models:

```bash
cd Software/src
python evaluate_models.py
```

Generates:
- Confusion matrices for each model
- Model comparison plots
- Comprehensive evaluation report with accuracy, precision, recall, F1-score

### 6. Configure ESP32

Edit `Embedded/src/config/config.h`:

```cpp
#define WIFI_SSID "YOUR_WIFI_SSID"
#define WIFI_PASSWORD "YOUR_WIFI_PASSWORD"
#define SERVER_IP "OUR_IP"  // Your computer's IP
#define SERVER_PORT 5000
```

Or use the template: `Embedded/src/config/config.h.template`

### 7. Compile and Upload to ESP32

#### Option A: Using Batch Script (Windows)

```bash
cd Embedded
compile_and_upload_autonomous.bat
```

This script:
1. Compiles code with PlatformIO
2. Prompts for ESP32 upload
3. Handles download mode instructions

**If upload fails:**
1. Hold the **BOOT** button
2. Press and release **RESET** button
3. Release **BOOT** button
4. Try upload again

#### Option B: Using PlatformIO CLI

```bash
cd Embedded
pio run              # Compile
pio run -t upload    # Upload
pio device monitor    # Serial monitor
```

#### Option C: Using Arduino IDE

1. Open `Embedded/autonomous_Driving/autonomous_Driving.ino` in Arduino IDE
2. Install ESP32 board support (ESP32 Dev Module)
3. Install required libraries:
   - NewPing
   - Encoder
   - OneWire
   - DallasTemperature
   - Adafruit INA219
   - Adafruit MPU6050
   - Adafruit ST7735
   - Adafruit GFX
4. Select board: **ESP32 Dev Module**
5. Select port: **COM4** (or your port)
6. Upload

### 8. Start Data Receiver Server

```bash
cd Software/data
python data_receiver.py
```

Server runs on `http://0.0.0.0:5000` (configurable via `.env`)

Endpoints:
- `POST /api/data` - Receive sensor data from ESP32
- `POST /api/predict` - Get ML prediction for sensor data
- `GET /api/status` - Check server and model status

### 9. Start Data Query API (Optional)

```bash
cd Software/data
python data_api.py
```

API runs on `http://0.0.0.0:5001` (configurable via `DATA_API_PORT` in `.env`)

Endpoints:
- `GET /api/data/records` - Query driving behavior records
- `GET /api/data/stats` - Statistical summary
- `GET /api/data/levels` - Level distribution
- `GET /api/data/record/<id>` - Get specific record
- `GET /api/data/export` - Export as CSV
- `GET /api/data/status` - API health check

## 📡 API Documentation

### Data Receiver API (Port 5000)

#### POST /api/data
Receive sensor data from ESP32 and save to database.

**Request Body:**
```json
{
  "temperature": 25.5,
  "voltage": 12.3,
  "current": 1.2,
  "power": 14.76,
  "accelX": 0.1,
  "accelY": 0.2,
  "accelZ": 9.8,
  "gyroX": 0.0,
  "gyroY": 0.0,
  "gyroZ": 0.0,
  "distance": 45.0,
  "speed": 0.5,
  "irDigital": 0,
  "irAnalog": 512
}
```

**Response:**
```json
{
  "status": "success",
  "message": "Data saved"
}
```

#### POST /api/predict
Get ML prediction for sensor data.

**Request Body:**
```json
{
  "voltage": 12.3,
  "current": 1.2,
  "temperature": 25.5,
  "accelX": 0.1,
  "accelY": 0.2,
  "accelZ": 9.8,
  "speed": 0.5,
  "distance": 45.0
}
```

**Response:**
```json
{
  "status": "success",
  "predicted_level": 7,
  "confidence": 0.92
}
```

#### GET /api/status
Check server and model status.

**Response:**
```json
{
  "status": "running",
  "model_loaded": true,
  "scaler_loaded": true
}
```

### Data Query API (Port 5001)

#### GET /api/data/records
Query driving behavior records with filtering.

**Query Parameters:**
- `limit` (default: 100) - Number of records
- `offset` (default: 0) - Pagination offset
- `level` - Filter by performance level (1-10)
- `min_voltage` - Minimum voltage filter
- `max_voltage` - Maximum voltage filter

**Example:**
```
GET /api/data/records?limit=50&level=7&min_voltage=12.0
```

#### GET /api/data/stats
Get statistical summary of database.

**Response:**
```json
{
  "total_records": 1000,
  "levels": {
    "1": 50,
    "2": 75,
    ...
  },
  "avg_voltage": 12.3,
  "avg_current": 1.2
}
```

## 📁 Project Structure

```
PowerTrain AI/
├── Embedded/                          # ESP32 firmware
│   ├── src/
│   │   ├── main.cpp                  # Main program entry point
│   │   ├── ESP32_car.h               # Core library includes
│   │   ├── config/
│   │   │   ├── config.h             # Configuration (WiFi, pins, speeds)
│   │   │   └── config.h.template    # Configuration template
│   │   ├── sensors/
│   │   │   ├── DS18B20_temp.cpp     # Temperature sensor driver
│   │   │   ├── HC-SRO4_Infrared.cpp # Ultrasonic distance sensor
│   │   │   ├── I2C_BUS.cpp          # I2C bus initialization
│   │   │   ├── IR_MH_Sensor.cpp     # Infrared obstacle sensor
│   │   │   ├── TB6612FNG_MotorStepper.cpp  # Motor driver
│   │   │   ├── TFT_LED.cpp          # TFT display functions
│   │   │   ├── TFT_LED.h            # TFT display header
│   │   │   └── voltage_sensor.cpp   # Voltage detection
│   │   ├── autonomous_drive.h       # Basic autonomous driving header
│   │   ├── autonomous_drive_ml.h    # ML-enhanced driving header
│   │   ├── autonomous_drive_ml.cpp  # ML-enhanced driving implementation
│   │   ├── ml_predictor.h           # ML prediction API header
│   │   └── ml_predictor.cpp         # HTTP client for ML predictions
│   ├── autonomous_Driving/
│   │   └── autonomous_Driving.ino   # Standalone autonomous sketch
│   ├── bluetooth_Driving/
│   │   └── bluetooth_Driving.ino    # Bluetooth control sketch
│   ├── compile_and_upload_autonomous.bat  # Build and upload script
│   └── platformio.ini               # PlatformIO configuration
│
├── Software/                         # Python ML and data processing
│   ├── data/
│   │   ├── raw_data/                # Raw sensor data from ESP32
│   │   │   └── *.jsonl, *.csv, *.db, *.parquet, *.h5, *.xlsx
│   │   ├── database/
│   │   │   └── driving_behavior.db   # SQLite database
│   │   ├── data_receiver.py         # Flask server for data reception
│   │   ├── data_api.py              # RESTful API for data queries
│   │   ├── check_security.py        # Security scanner
│   │   ├── SECURITY.md              # Security guidelines
│   │   ├── driving_behavior_dataset.csv  # Training dataset
│   │   └── data.pdf                 # Data documentation
│   ├── models/                      # Trained ML models
│   │   ├── mlp_model.joblib        # scikit-learn MLP model
│   │   ├── scaler.joblib           # Feature scaler for MLP
│   │   ├── random_forest_model.joblib
│   │   ├── random_forest_scaler.joblib
│   │   ├── pytorch_nn_model.pth    # PyTorch neural network
│   │   ├── pytorch_scaler.joblib
│   │   └── results/                # Evaluation plots and reports
│   ├── src/
│   │   ├── create_database.py      # Create SQLite database from CSV
│   │   ├── preprocessing.py          # Process raw data for ML
│   │   ├── save_model.py            # Train and save all three models
│   │   ├── train_pytorch_nn.py      # Train PyTorch neural network
│   │   ├── train_random_forest.py   # Train Random Forest
│   │   ├── train_mlp.py            # Train scikit-learn MLP
│   │   ├── evaluate_models.py       # Evaluate all models
│   │   └── model.py                # Original MLP training script
│   ├── requirements.txt             # Python dependencies
│   └── README.md                    # Software module documentation
│
├── scripts/                         # Utility scripts
│   ├── install_platformio.ps1      # PlatformIO installation (Windows)
│   └── install_platformio.sh       # PlatformIO installation (Linux/Mac)
│
├── images/                          # Project images
│   ├── car.jpg
│   ├── carSideways.jpg
│   ├── carTraining.jpg
│   ├── buildingBeginning.jpg
│   └── circuitBuilding.jpg
│
├── Modelling/                       # 3D models
│   ├── base_modelling_final.SLDPRT
│   ├── base_modelling_intial.SLDPRT
│   └── solidworks_Final.jpg
│
├── Schematics/                      # Circuit schematics
│   ├── Schematics.jpg
│   └── Schematics_start.jpg
│
├── run_model.bat                    # Run ML model training
├── .gitignore                       # Git ignore rules
├── PLATFORMIO_SETUP.md              # PlatformIO setup guide
└── README.md                        # This file
```

## 🔄 Data Flow

### Training Phase

1. **Data Collection**: ESP32 collects sensor data during autonomous driving
2. **Data Transmission**: Sensor data sent via WiFi to Flask server (`data_receiver.py`)
3. **Data Storage**: Data saved in multiple formats (CSV, JSON, SQLite, etc.)
4. **Data Preprocessing**: `preprocessing.py` calculates derived features:
   - Internal resistance (from voltage drop and current)
   - Combined acceleration magnitude
   - Cumulative distance
   - Performance level assignment (1-10)
5. **Database Import**: `create_database.py` imports processed data to SQLite
6. **Model Training**: `save_model.py` trains all three models:
   - Features: InternalResistance, StartingInternalResistance, Voltage, Current, Temp, AccelX, AccelY, AccelCombined, Speed, Distance
   - Target: Level (1-10 performance classification)
   - Train-test split: 80-20 with stratification
   - Feature scaling: StandardScaler
7. **Model Evaluation**: `evaluate_models.py` compares all models and generates reports

### Inference Phase (Real-Time)

1. **Sensor Reading**: ESP32 reads all sensors (temperature, voltage, current, acceleration, distance, speed)
2. **Data Preprocessing**: ESP32 calculates internal resistance and combined acceleration
3. **HTTP Request**: ESP32 sends sensor data to `/api/predict` endpoint
4. **ML Prediction**: Python server:
   - Loads trained model and scaler
   - Preprocesses input data
   - Makes prediction (level 1-10)
   - Returns prediction and confidence
5. **Autonomous Decision**: ESP32 uses prediction to:
   - Adjust speed based on performance level
   - Modify obstacle detection thresholds
   - Optimize driving behavior

## 🎯 Autonomous Driving Modes

### Basic Autonomous Mode

Uses sensor-based obstacle avoidance:
- Forward movement with continuous distance monitoring
- Turn left/right based on clearer path detection
- Backup and turn when completely blocked
- Speed control via Serial commands (L/M/H/S)

### ML-Enhanced Autonomous Mode

Integrates ML predictions for intelligent behavior:
- **Performance-Based Speed**: Adjusts speed based on predicted performance level
  - Level 1-3: Low speed (conservative)
  - Level 4-7: Medium speed (balanced)
  - Level 8-10: High speed (aggressive)
- **Adaptive Thresholds**: Obstacle detection thresholds adjust based on confidence
- **Predictive Behavior**: Anticipates performance degradation and adjusts proactively

## 📊 Model Training Results

All models are trained on the same dataset with:
- **Features**: 10 input features (InternalResistance, Voltage, Current, Temp, AccelX, AccelY, AccelCombined, Speed, Distance, StartingInternalResistance)
- **Target**: Performance level (1-10 classification)
- **Train-Test Split**: 80-20 with stratification
- **Evaluation Metrics**: Accuracy, Precision, Recall, F1-Score, Confusion Matrix

Run `evaluate_models.py` to see detailed results for each model.

## 🚀 Running Batch Scripts

### run_model.bat

Trains the MLP model and generates visualization:

```bash
run_model.bat
```

This script:
1. Checks Python installation
2. Installs required packages
3. Runs `train_mlp.py`
4. Generates confusion matrix visualization
5. Saves results to `Software/models/results/`

### compile_and_upload_autonomous.bat

Compiles and uploads autonomous driving code to ESP32:

```bash
cd Embedded
compile_and_upload_autonomous.bat
```

This script:
1. Compiles code with PlatformIO
2. Prompts for ESP32 upload
3. Provides instructions for download mode if needed

**Note**: Make sure ESP32 is connected to COM4 (or update `platformio.ini` with your port).

## 🔧 Troubleshooting

### ESP32 Upload Issues

**Problem**: Upload fails with "Wrong boot mode detected"

**Solution**:
1. Hold the **BOOT** button
2. Press and release **RESET** button
3. Release **BOOT** button
4. Try upload again within 10 seconds

### WiFi Connection Issues

**Problem**: ESP32 cannot connect to WiFi

**Solution**:
1. Check WiFi credentials in `Embedded/src/config/config.h`
2. Verify WiFi network is 2.4GHz (ESP32 doesn't support 5GHz)
3. Check signal strength
4. Review Serial monitor for connection errors

### ML Predictions Not Working

**Problem**: ESP32 cannot get predictions from server

**Solution**:
1. Ensure `data_receiver.py` is running on port 5000
2. Check server IP in `config.h` matches your computer's IP
3. Verify models are loaded (check `/api/status` endpoint)
4. Check firewall settings allow port 5000

### Database Not Found

**Problem**: Models cannot find database

**Solution**:
```bash
cd Software/src
python create_database.py
```

### Model Training Errors

**Problem**: Training fails with missing data

**Solution**:
1. Ensure database exists: `Software/data/database/driving_behavior.db`
2. Verify database has data: Check with SQLite browser
3. Run `create_database.py` to import CSV data

### Sensor Reading Issues

**Problem**: Sensors not reading correctly

**Solution**:
1. Verify I2C connections (SDA=21, SCL=22)
2. Check sensor addresses with I2C scanner
3. Verify power supply (3.3V for sensors)
4. Check GPIO pin assignments in `config.h`

## Support

For questions, issues, or contributions:

- **Repository**: [PowerTrain AI](https://github.com/XavierS31/PowerTrain-AI)
- **Issues**: Open an issue on GitHub

## 🎯 Future Enhancements

- Real-time model retraining with new data
- Advanced sensor fusion algorithms
- Computer vision integration for object recognition
- GPS-based navigation and path planning
- Mobile app for remote monitoring and control
- Cloud deployment for ML inference
- Enhanced battery health prediction
- Multi-car fleet management
- Advanced obstacle avoidance with LiDAR
- Reinforcement learning for adaptive behavior

## 📝 License

This project is open source and available for educational and research purposes.

## 🙏 Acknowledgments

- ESP32 community for excellent documentation
- PlatformIO for seamless embedded development
- PyTorch and scikit-learn teams for ML frameworks
- Adafruit for sensor libraries and hardware

---

**Built with passion for autonomous systems and machine learning** ⚡🚗
