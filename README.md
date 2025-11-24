# PowerTrain AI - ESP32 Autonomous Car Project

## 🚗 Project Overview

Autonomous car with obstacle avoidance, sensor data logging, and WiFi communication using ESP32 DevKit V1.

## 📁 Project Structure

```
PowerTrain AI/
├── Embedded/              # ESP32 firmware (PlatformIO)
│   ├── src/              # Source code
│   │   ├── main.cpp      # Main program
│   │   ├── config/       # Configuration (WiFi, etc.)
│   │   └── sensors/       # Sensor drivers
│   ├── platformio.ini    # PlatformIO configuration
│   └── QUICK_START.md    # Quick reference guide
├── Software/             # Python data processing
│   ├── data/            # Data receiver server
│   └── src/             # ML/data processing code
├── scripts/              # Utility scripts
│   ├── generate_config.py    # Generate ESP32 config from .env
│   └── install_platformio.*  # PlatformIO installation
└── .env                  # Environment variables (WiFi, IP, etc.)
```

## 🚀 Quick Start

### 1. Setup Environment

```powershell
# Create .env file (copy from .env.example if needed)
# Edit .env with your WiFi credentials and computer IP

# Generate ESP32 config
python scripts/generate_config.py
```

### 2. Install PlatformIO

```powershell
.\scripts\install_platformio.ps1
```

### 3. Build & Upload

```powershell
cd Embedded
pio run                    # Build
pio run -t upload          # Upload to ESP32
pio device monitor         # Serial monitor
```

**Or use VS Code tasks:** `Ctrl+Shift+P` → `Tasks: Run Task`

### 4. Start Data Receiver

```powershell
cd Software/data
python data_receiver.py
```

## 📚 Documentation

- **Embedded Module:** `Embedded/README.md`
- **Software Module:** `Software/README.md`
- **Full Setup Guide:** `PLATFORMIO_SETUP.md`
- **Security Guide:** `Software/data/SECURITY.md`

## 🔧 Features

- ✅ Autonomous driving with obstacle avoidance
- ✅ Three speed levels (Low/Medium/High)
- ✅ WiFi sensor data logging (5 readings/sec)
- ✅ TFT display showing performance (0-100%)
- ✅ Multiple sensors: Temperature, Voltage, Current, MPU6050, HC-SR04, IR

## 🛠️ Development

**PlatformIO** is the primary development environment. All code compiles and uploads directly in Cursor IDE.

**VS Code Tasks:**
- `Ctrl+Shift+B` - Build
- `Ctrl+Shift+P` → Tasks → Upload/Monitor

## 📝 Configuration

Edit `.env` file for:
- WiFi SSID/Password
- Server IP/Port
- Motor speeds
- Sensor thresholds

Then run: `python scripts/generate_config.py`

## 🔗 Links

- [PlatformIO Documentation](https://docs.platformio.org/)
- [ESP32 Platform](https://docs.platformio.org/en/latest/platforms/espressif32.html)
