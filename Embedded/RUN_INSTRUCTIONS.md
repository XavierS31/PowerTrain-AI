# 🚗 How to Compile, Upload, and Run ESP32 Car

## Quick Steps

### 1. **Start Data Receiver Server** (Optional - for data logging)
Open a terminal in Cursor and run:
```powershell
cd Software/data
python data_receiver.py
```
Keep this running in the background. It will receive sensor data via WiFi.

### 2. **Compile the Code**
In Cursor terminal:
```powershell
cd Embedded
pio run
```
**Or use keyboard shortcut:** `Ctrl+Shift+B`

This compiles your code and checks for errors.

### 3. **Upload to ESP32**
**IMPORTANT:** Make sure your ESP32 is connected via USB to COM4 (or update `upload_port` in `platformio.ini`)

```powershell
cd Embedded
pio run -t upload
```

**If upload fails:**
- Put ESP32 in download mode:
  1. Hold the **BOOT** button
  2. Press and release **RESET** button
  3. Release **BOOT** button
- Then try upload again

**Or use VS Code task:**
- Press `Ctrl+Shift+P`
- Type: `Tasks: Run Task`
- Select: `PlatformIO: Upload`

### 4. **Monitor Serial Output** (Optional - for debugging)
```powershell
cd Embedded
pio device monitor
```

This shows:
- Sensor readings
- WiFi connection status
- Motor control messages
- Performance metrics

**Or use VS Code task:**
- Press `Ctrl+Shift+P`
- Type: `Tasks: Run Task`
- Select: `PlatformIO: Monitor`

### 5. **Unplug and Run Standalone** ✅
**YES, you can unplug after uploading!**

Once the code is uploaded:
1. The ESP32 will run **autonomously** - no laptop needed
2. The car will start driving immediately on boot
3. It will avoid obstacles using HC-SR04 sensor
4. If WiFi is connected, it will send data to your computer
5. If WiFi is not connected, it will still drive (just won't log data)

**To run completely standalone:**
- Upload the code
- Unplug USB cable
- Power ESP32 with battery/power supply
- Car will start driving automatically!

## Complete Workflow

```powershell
# Terminal 1: Start data receiver (optional)
cd Software/data
python data_receiver.py

# Terminal 2: Compile and upload
cd Embedded
pio run                    # Compile
pio run -t upload          # Upload to ESP32

# Optional: Monitor serial output
pio device monitor

# Then unplug and let it run!
```

## What Happens After Upload

1. **On Boot:**
   - TFT display initializes and shows "Car Driving Behavior Performance"
   - Sensors initialize (INA219, MPU6050, encoders, etc.)
   - WiFi connects (if configured)
   - **Car starts driving forward immediately!**

2. **During Operation:**
   - Car drives forward at medium speed
   - HC-SR04 detects obstacles and turns left/right
   - All sensors read data 5 times per second
   - Data sent to computer via WiFi (if connected)
   - TFT display updates performance (0-100%)

3. **Speed Control:**
   - Send `L`, `M`, or `H` via Serial Monitor for Low/Medium/High speed
   - Send `S` to stop

## Troubleshooting

**"Upload failed"**
- Check COM port: `pio device list`
- Update `upload_port` in `platformio.ini` if needed
- Put ESP32 in download mode (hold BOOT, press RESET, release BOOT)

**"Car not moving"**
- Check motor connections
- Verify STBY pin is HIGH
- Check Serial Monitor for error messages

**"No data received"**
- Check WiFi connection on ESP32
- Verify computer IP in `.env` file
- Make sure data receiver is running
- Check firewall settings

**"TFT screen blank"**
- Check SPI connections
- Verify TFT pins in code match your wiring

## File Locations

- **Main code:** `Embedded/src/main.cpp`
- **Config:** `Embedded/src/config/config.h` (generate with `python scripts/generate_config.py`)
- **PlatformIO config:** `Embedded/platformio.ini`
- **Data receiver:** `Software/data/data_receiver.py`
- **Data saved to:** `Software/data/raw_data/`

## Summary

✅ **Compile:** `pio run` or `Ctrl+Shift+B`  
✅ **Upload:** `pio run -t upload`  
✅ **Monitor:** `pio device monitor` (optional)  
✅ **Unplug:** YES! Car runs standalone after upload  
✅ **Data logging:** Start `data_receiver.py` first (optional)

