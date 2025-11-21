# PlatformIO Setup Guide for ESP32 Development in Cursor

This guide will help you set up a complete ESP32 development environment inside Cursor IDE using PlatformIO.

## 🎯 What You Get

- ✅ **Compile and upload directly in Cursor** - No Arduino IDE needed
- ✅ **Serial Monitor in Cursor terminal** - See debug output instantly
- ✅ **Automatic library management** - All dependencies handled
- ✅ **Debug support** - Optional JTAG debugging
- ✅ **VS Code tasks** - One-click build/upload/monitor

---

## 📋 Prerequisites

- **Python 3.7+** installed
- **ESP32 DevKit V1** connected via USB
- **Cursor IDE** (or VS Code)

---

## 🚀 Quick Setup (5 Minutes)

### Step 1: Install PlatformIO

**Windows (PowerShell):**
```powershell
cd "C:\Users\Xavie\OneDrive\Desktop\PowerTrain AI"
.\scripts\install_platformio.ps1
```

**Linux/Mac:**
```bash
cd ~/PowerTrain AI
chmod +x scripts/install_platformio.sh
./scripts/install_platformio.sh
```

**Or install manually:**
```bash
pip install platformio
pio platform install espressif32
```

### Step 2: Verify Installation

```powershell
# Check PlatformIO is installed
pio --version

# Should show: PlatformIO Core, version x.x.x
```

### Step 3: Build Your Project

```powershell
cd Embedded
pio run
```

**Expected output:**
```
Building in release mode
...
Linking .pio/build/esp32dev/firmware.elf
...
SUCCESS
```

---

## 🛠️ Development Workflow

### Build (Compile Only)

**Option 1: Terminal Command**
```powershell
cd Embedded
pio run
```

**Option 2: VS Code Task**
- Press `Ctrl+Shift+P` (or `Cmd+Shift+P` on Mac)
- Type: `Tasks: Run Task`
- Select: `PlatformIO: Build`

**Option 3: Keyboard Shortcut**
- Press `Ctrl+Shift+B` (builds default task)

### Upload to ESP32

**Option 1: Terminal Command**
```powershell
cd Embedded
pio run -t upload
```

**Option 2: VS Code Task**
- `Ctrl+Shift+P` → `Tasks: Run Task` → `PlatformIO: Upload`

**Option 3: Upload & Monitor (Recommended)**
- `Ctrl+Shift+P` → `Tasks: Run Task` → `PlatformIO: Upload & Monitor`

### Serial Monitor

**Option 1: Terminal Command**
```powershell
cd Embedded
pio device monitor
```

**Option 2: VS Code Task**
- `Ctrl+Shift+P` → `Tasks: Run Task` → `PlatformIO: Monitor`

**Exit monitor:** Press `Ctrl+]` or `Ctrl+C`

### Clean Build

```powershell
cd Embedded
pio run -t clean
```

---

## 📁 Project Structure

```
Embedded/
├── platformio.ini          # PlatformIO configuration
├── src/
│   ├── main.cpp            # Main program (converted from .ino)
│   ├── ESP32_car.h         # Sensor headers
│   ├── config/
│   │   └── config.h        # WiFi & settings
│   └── sensors/
│       ├── TFT_LED.h
│       ├── TFT_LED.cpp
│       └── ...
└── .pio/                   # Build artifacts (auto-generated)
```

---

## 🔧 Configuration

### platformio.ini

The configuration file is already set up with:
- ✅ ESP32 DevKit V1 board
- ✅ All required libraries (MPU6050, INA219, TFT ST7735, etc.)
- ✅ Serial monitor settings (115200 baud)
- ✅ Upload speed optimized

**Key settings:**
```ini
[env:esp32dev]
platform = espressif32
board = esp32dev
framework = arduino
monitor_speed = 115200
```

### Libraries Included

All your sensor libraries are automatically installed:
- `ArduinoJson` - JSON serialization
- `OneWire` & `DallasTemperature` - DS18B20 temperature
- `ESP32AnalogRead` - Voltage sensor
- `Adafruit INA219` - Current/power sensor
- `Adafruit MPU6050` - Accelerometer/gyroscope
- `NewPing` - HC-SR04 ultrasonic
- `Adafruit ST7735` - TFT display
- And more...

---

## 🐛 Troubleshooting

### "pio: command not found"

**Windows:**
- Restart PowerShell/terminal
- Check Python Scripts is in PATH: `C:\Users\YourName\AppData\Local\Programs\Python\PythonXX\Scripts`

**Linux/Mac:**
- Add to PATH: `export PATH=$PATH:~/.platformio/penv/bin`
- Or use: `python3 -m platformio`

### "Upload failed"

1. **Check COM port:**
   ```powershell
   pio device list
   ```

2. **Update upload_port in platformio.ini:**
   ```ini
   upload_port = COM3  ; Windows
   upload_port = /dev/ttyUSB0  ; Linux
   upload_port = /dev/cu.usbserial-*  ; Mac
   ```

3. **Try holding BOOT button** during upload

### "Library not found"

PlatformIO will auto-download libraries. If issues persist:
```powershell
cd Embedded
pio lib install
```

### "Compilation errors"

1. **Check include paths** - All headers should be in `src/`
2. **Verify config.h exists:**
   ```powershell
   python scripts/generate_config.py
   ```

3. **Clean and rebuild:**
   ```powershell
   pio run -t clean
   pio run
   ```

---

## 🎮 VS Code Tasks Reference

| Task | Command | Shortcut |
|------|---------|----------|
| Build | `PlatformIO: Build` | `Ctrl+Shift+B` |
| Upload | `PlatformIO: Upload` | - |
| Upload & Monitor | `PlatformIO: Upload & Monitor` | - |
| Monitor | `PlatformIO: Monitor` | - |
| Clean | `PlatformIO: Clean` | - |

**Access tasks:**
- `Ctrl+Shift+P` → `Tasks: Run Task`
- Or use Command Palette

---

## 🔍 Debugging (Optional)

### Setup JTAG Debugging

1. **Install debug tools:**
   ```powershell
   pio platform install espressif32
   ```

2. **Connect JTAG adapter** (ESP-Prog, Olimex, etc.)

3. **Use debug environment:**
   ```powershell
   pio run -e esp32dev-debug
   ```

4. **Launch debugger:**
   - `F5` or `Run > Start Debugging`
   - Set breakpoints in code
   - Step through execution

---

## 📊 Serial Monitor Tips

### Filter Output

Edit `platformio.ini`:
```ini
monitor_filters = 
    default
    time        ; Add timestamps
    colorize    ; Color output
```

### Change Baud Rate

```ini
monitor_speed = 115200  ; Change to your preferred rate
```

### Send Commands

In monitor, type commands:
- `L` - Low speed
- `M` - Medium speed
- `H` - High speed
- `S` - Stop

---

## 🆚 PlatformIO vs Arduino IDE

| Feature | PlatformIO | Arduino IDE |
|---------|------------|--------------|
| Compile in Cursor | ✅ Yes | ❌ No |
| Library Management | ✅ Automatic | ⚠️ Manual |
| Serial Monitor | ✅ Built-in | ⚠️ Separate |
| Debug Support | ✅ Yes | ❌ Limited |
| Project Structure | ✅ Professional | ⚠️ Simple |
| Learning Curve | ⚠️ Steeper | ✅ Easy |

**Recommendation:** Use PlatformIO for development, Arduino IDE for quick testing.

---

## 📝 Common Commands Cheat Sheet

```powershell
# Navigate to project
cd Embedded

# Build
pio run

# Upload
pio run -t upload

# Monitor
pio device monitor

# Upload + Monitor (one command)
pio run -t upload && pio device monitor

# Clean
pio run -t clean

# List devices
pio device list

# Update libraries
pio lib update

# Check library dependencies
pio lib list
```

---

## ✅ Verification Checklist

- [ ] PlatformIO installed (`pio --version`)
- [ ] ESP32 platform installed (`pio platform list`)
- [ ] Project builds successfully (`pio run`)
- [ ] Can upload to ESP32 (`pio run -t upload`)
- [ ] Serial monitor works (`pio device monitor`)
- [ ] VS Code tasks work (`Ctrl+Shift+P` → Tasks)

---

## 🎉 You're Ready!

Your ESP32 development environment is now fully set up in Cursor. You can:

1. **Edit code** in Cursor with full IntelliSense
2. **Build** with `Ctrl+Shift+B`
3. **Upload** via tasks or terminal
4. **Monitor** serial output in Cursor terminal
5. **Debug** with breakpoints (optional)

**Happy coding! 🚀**

---

## 📚 Additional Resources

- [PlatformIO Documentation](https://docs.platformio.org/)
- [ESP32 Platform](https://docs.platformio.org/en/latest/platforms/espressif32.html)
- [PlatformIO VS Code Extension](https://marketplace.visualstudio.com/items?itemName=platformio.platformio-ide)

