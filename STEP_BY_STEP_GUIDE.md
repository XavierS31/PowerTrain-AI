# 📋 Step-by-Step Guide: Compile and Upload to ESP32

## 🎯 Complete Process Overview

1. **Prepare ESP32** (put in download mode)
2. **Compile code** (check for errors)
3. **Upload code** (flash to ESP32)
4. **Verify** (check if it works)

---

## 📝 Detailed Steps

### **STEP 1: Prepare Your ESP32**

1. **Connect ESP32 to your computer**
   - Use USB cable
   - Make sure it's connected to **COM4** (or check your port in `Embedded/platformio.ini`)

2. **Put ESP32 in Download Mode** ⚠️ **CRITICAL STEP**
   - **Hold the BOOT button** (keep holding it)
   - **Press and release the RESET button** (while still holding BOOT)
   - **Release the BOOT button**
   - ESP32 is now in download mode (ready for upload)

   **Note:** You have about 10 seconds to start the upload after putting it in download mode.

---

### **STEP 2: Compile the Code**

**Option A: Using Batch File (Easiest)**
1. Navigate to `Embedded` folder
2. Double-click `compile.bat`
3. Wait for compilation to complete
4. Look for: `[SUCCESS]` message

**Option B: Using Command Line**
```powershell
cd Embedded
pio run
```

**What to look for:**
- ✅ `[SUCCESS]` = Compilation successful
- ❌ `[FAILED]` = Check error messages

---

### **STEP 3: Upload to ESP32**

**IMPORTANT:** Make sure ESP32 is in download mode (from Step 1)!

**Option A: Using Batch File (Easiest)**
1. Make sure ESP32 is in download mode (Step 1)
2. Navigate to `Embedded` folder
3. Double-click `compile_and_upload.bat`
4. When prompted, press any key to continue
5. Wait for upload to complete

**Option B: Using Master Batch File (All-in-One)**
1. Put ESP32 in download mode (Step 1)
2. Double-click `MASTER_RUN.bat` in project root
3. Follow the prompts
4. When asked about data receiver, type `y` or `n`
5. When prompted to upload, press any key

**Option C: Using Command Line**
```powershell
cd Embedded
pio run -t upload
```

**What to look for:**
- ✅ `Writing at 0x00010000... (100 %)` = Upload successful
- ✅ `Hash of data verified` = Upload verified
- ❌ `Failed to connect` = ESP32 not in download mode (try Step 1 again)
- ❌ `Wrong boot mode` = Put ESP32 in download mode again

---

### **STEP 4: Verify Upload**

1. **Press RESET button** on ESP32 (to restart it)
2. **Check Serial Monitor** (optional):
   - Double-click `Embedded/monitor.bat`
   - Or run: `cd Embedded` then `pio device monitor`
   - You should see:
     - "TFT Display initialized"
     - "INA219 initialized"
     - "MPU6050 initialized"
     - "Autonomous car ready!"
     - "Starting autonomous driving..."

3. **Check TFT Display** (if connected):
   - Should show "Car Driving Behavior Performance"
   - Should display 0-100% performance

4. **Check Car Movement**:
   - Car should start driving forward automatically
   - Should avoid obstacles using HC-SR04 sensor

---

## 🔄 Complete Workflow (Quick Reference)

### **Method 1: All-in-One (Recommended)**
```
1. Put ESP32 in download mode (BOOT → RESET → release BOOT)
2. Double-click: MASTER_RUN.bat
3. Answer prompts
4. Wait for "UPLOAD SUCCESSFUL!"
5. Press RESET on ESP32
6. Done! Car starts driving
```

### **Method 2: Step-by-Step**
```
1. Put ESP32 in download mode
2. Double-click: Embedded/compile_and_upload.bat
3. Press any key when prompted
4. Wait for upload
5. Press RESET on ESP32
6. Done!
```

---

## ⚠️ Troubleshooting

### **Problem: "Failed to connect to ESP32"**
**Solution:**
- Put ESP32 in download mode again
- Make sure USB cable is connected
- Check COM port in `platformio.ini` (should be COM4)

### **Problem: "Wrong boot mode detected"**
**Solution:**
- ESP32 is not in download mode
- Repeat Step 1 (BOOT → RESET → release BOOT)
- Start upload immediately after

### **Problem: "Upload failed" or "Chip stopped responding"**
**Solution:**
- Try a different USB cable
- Try a different USB port
- Put ESP32 in download mode again
- Reduce upload speed in `platformio.ini` (change `upload_speed = 115200` to `upload_speed = 921600`)

### **Problem: "Compilation failed"**
**Solution:**
- Check error messages
- Make sure all libraries are installed: `pio lib install`
- Check `platformio.ini` configuration

### **Problem: "Car not moving after upload"**
**Solution:**
- Press RESET button on ESP32
- Check Serial Monitor for error messages
- Verify motor connections
- Check STBY pin is HIGH

---

## 📊 What Happens After Successful Upload

1. **On Boot:**
   - TFT display initializes
   - All sensors initialize (INA219, MPU6050, encoders, etc.)
   - WiFi connects (if configured)
   - **Car starts driving forward automatically!**

2. **During Operation:**
   - Car drives forward at medium speed
   - HC-SR04 detects obstacles and turns left/right
   - All sensors read data 5 times per second
   - Data sent to computer via WiFi (if connected)
   - TFT display shows performance (0-100%)

3. **Standalone Operation:**
   - You can unplug USB cable
   - Car will run on battery/power supply
   - No laptop needed!

---

## 🎮 Serial Monitor Commands

After upload, you can send commands via Serial Monitor:

- **`L`** = Low speed
- **`M`** = Medium speed (default)
- **`H`** = High speed
- **`S`** = Stop motors

---

## 📁 File Locations

- **Main code:** `Embedded/src/main.cpp`
- **Config:** `Embedded/src/config/config.h` (generate with `python scripts/generate_config.py`)
- **PlatformIO config:** `Embedded/platformio.ini`
- **Batch files:** 
  - `MASTER_RUN.bat` (all-in-one)
  - `Embedded/compile_and_upload.bat` (compile + upload)
  - `Embedded/monitor.bat` (Serial Monitor)

---

## ✅ Success Checklist

After upload, verify:
- [ ] Compilation successful
- [ ] Upload successful (no errors)
- [ ] ESP32 restarts (press RESET)
- [ ] Serial Monitor shows initialization messages
- [ ] TFT display shows performance (if connected)
- [ ] Car starts driving forward
- [ ] Obstacle avoidance works (HC-SR04)
- [ ] Data being sent to computer (if WiFi connected)

---

## 🚀 Quick Start (Copy-Paste Ready)

```powershell
# 1. Put ESP32 in download mode (manual - hold BOOT, press RESET, release BOOT)

# 2. Compile and upload
cd Embedded
pio run -t upload

# 3. Monitor (optional)
pio device monitor
```

**Or just double-click:** `MASTER_RUN.bat` 🎉

---

## 💡 Pro Tips

1. **Always put ESP32 in download mode BEFORE starting upload**
2. **Keep Serial Monitor open** to see what's happening
3. **Check TFT display** to verify code is running
4. **Test obstacle avoidance** by placing object in front of car
5. **Data logging:** Start `Software/data/start_data_receiver.bat` before uploading

---

**Need help?** Check error messages in Serial Monitor or compilation output!


┌─────────────────────────────────────────┐
│  STEP 1: Put ESP32 in Download Mode     │
│  ─────────────────────────────────────   │
│  1. Hold BOOT button (keep holding)      │
│  2. Press and release RESET button       │
│  3. Release BOOT button                   │
│  ✅ ESP32 ready for upload!              │
└─────────────────────────────────────────┘
              ↓
┌─────────────────────────────────────────┐
│  STEP 2: Compile Code                    │
│  ─────────────────────────────────────   │
│  Double-click: Embedded/compile.bat      │
│  OR run: cd Embedded && pio run         │
│  ✅ Wait for [SUCCESS] message           │
└─────────────────────────────────────────┘
              ↓
┌─────────────────────────────────────────┐
│  STEP 3: Upload to ESP32                │
│  ─────────────────────────────────────   │
│  Double-click: MASTER_RUN.bat            │
│  OR: Embedded/compile_and_upload.bat    │
│  ✅ Wait for "Upload successful"         │
└─────────────────────────────────────────┘
              ↓
┌─────────────────────────────────────────┐
│  STEP 4: Verify                          │
│  ─────────────────────────────────────   │
│  1. Press RESET button on ESP32          │
│  2. Car starts driving automatically!   │
│  3. You can unplug and run standalone   │
└─────────────────────────────────────────┘


