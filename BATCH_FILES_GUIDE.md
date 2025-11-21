# 🚀 Batch Files - Quick Reference

## 📁 Essential Batch Files (Only 4 Files!)

### **1. `MASTER_RUN.bat`** ⭐ **MAIN FILE - Use This!**
**Location:** Project root

**What it does:**
- Compiles ESP32 code
- Uploads to ESP32
- Optionally starts data receiver server
- **Complete setup in one click!**

**How to use:**
1. Put ESP32 in download mode (hold BOOT, press RESET, release BOOT)
2. Double-click `MASTER_RUN.bat`
3. Follow prompts
4. Done!

---

### **2. `Embedded/compile_and_upload.bat`**
**Location:** `Embedded/` folder

**What it does:**
- Compiles code
- Uploads to ESP32
- **Most commonly used for development**

**How to use:**
1. Put ESP32 in download mode
2. Double-click `compile_and_upload.bat`
3. Press any key when prompted
4. Done!

---

### **3. `Embedded/monitor.bat`**
**Location:** `Embedded/` folder

**What it does:**
- Opens Serial Monitor
- Shows sensor readings and debug messages
- **Useful for debugging**

**How to use:**
- Double-click `monitor.bat`
- Press Ctrl+C to exit

---

### **4. `Software/data/start_data_receiver.bat`**
**Location:** `Software/data/` folder

**What it does:**
- Starts Flask server to receive sensor data
- Saves data to CSV files
- **Use this if you want data logging**

**How to use:**
- Double-click `start_data_receiver.bat`
- Keep window open while ESP32 is running
- Data saved to: `Software/data/raw_data/`

---

## 🎯 Quick Workflow

### **Daily Development:**
```
1. Put ESP32 in download mode
2. Double-click: Embedded/compile_and_upload.bat
3. Done!
```

### **Complete Setup (First Time):**
```
1. Put ESP32 in download mode
2. Double-click: MASTER_RUN.bat
3. Answer prompts
4. Done!
```

### **With Data Logging:**
```
1. Double-click: Software/data/start_data_receiver.bat
2. Put ESP32 in download mode
3. Double-click: Embedded/compile_and_upload.bat
4. Done!
```

---

## 📊 File Summary

| File | Purpose | When to Use |
|------|---------|-------------|
| `MASTER_RUN.bat` | All-in-one setup | First time setup, complete workflow |
| `Embedded/compile_and_upload.bat` | Compile + Upload | Daily development (most used) |
| `Embedded/monitor.bat` | Serial Monitor | Debugging, see sensor data |
| `Software/data/start_data_receiver.bat` | Data logging | When you want to save sensor data |

---

## 💡 Pro Tips

1. **Most common:** Use `Embedded/compile_and_upload.bat` for regular development
2. **First time:** Use `MASTER_RUN.bat` for complete setup
3. **Debugging:** Use `Embedded/monitor.bat` to see what's happening
4. **Data logging:** Start `Software/data/start_data_receiver.bat` before uploading

---

**That's it! Only 4 essential batch files!** 🎉
