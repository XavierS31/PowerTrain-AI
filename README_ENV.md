# Environment Configuration Setup

This project uses a `.env` file to store sensitive configuration data that can be accessed by both the Embedded (ESP32) and Software (Python) components.

## Setup Instructions

### 1. Create .env File

Copy the example file and fill in your values:

```bash
cp .env.example .env
```

### 2. Edit .env File

Open `.env` and update the following values:

```env
# WiFi Configuration
WIFI_SSID=YourWiFiNetwork
WIFI_PASSWORD=YourWiFiPassword

# Server Configuration (your computer's IP address)
SERVER_IP=192.168.1.100
SERVER_PORT=5000
```

### 3. Generate ESP32 Config

After updating `.env`, generate the Arduino config file:

```bash
python scripts/generate_config.py
```

This creates `Embedded/src/config/config.h` with all the configuration values.

### 4. Update .gitignore

The `.env` file is already in `.gitignore` to prevent committing sensitive data.

## Usage

### For Python/Software

The Python code automatically loads `.env` using `python-dotenv`:

```python
from dotenv import load_dotenv
import os

load_dotenv()  # Loads .env from project root
wifi_ssid = os.getenv('WIFI_SSID')
server_port = int(os.getenv('SERVER_PORT', '5000'))
```

### For Arduino/ESP32

Include the generated config header:

```cpp
#include "src/config/config.h"

// Use the defines
const char* ssid = WIFI_SSID;
const char* password = WIFI_PASSWORD;
```

## Configuration Variables

| Variable | Description | Default |
|----------|-------------|---------|
| `WIFI_SSID` | WiFi network name | - |
| `WIFI_PASSWORD` | WiFi password | - |
| `SERVER_IP` | Computer IP address | 192.168.1.100 |
| `SERVER_PORT` | Server port | 5000 |
| `MOTOR_SPEED_LOW` | Low speed PWM (0-255) | 85 |
| `MOTOR_SPEED_MEDIUM` | Medium speed PWM | 170 |
| `MOTOR_SPEED_HIGH` | High speed PWM | 255 |
| `OBSTACLE_DISTANCE_CM` | Obstacle detection distance | 30 |
| `TURN_DURATION_MS` | Turn duration in milliseconds | 500 |
| `SENSOR_READ_INTERVAL_MS` | Sensor reading interval | 200 |
| `TFT_UPDATE_INTERVAL_MS` | TFT display update interval | 500 |
| `SAVE_JSON` | Save JSON format | true |
| `SAVE_CSV` | Save CSV format | false |
| `SAVE_SQLITE` | Save SQLite format | false |
| `SAVE_PARQUET` | Save Parquet format | false |
| `SAVE_HDF5` | Save HDF5 format | false |
| `SAVE_EXCEL` | Save Excel format | false |

## Security Notes

- **Never commit `.env` to version control**
- The `.env` file is already in `.gitignore`
- Share `.env.example` as a template instead
- Regenerate `config.h` after updating `.env`

