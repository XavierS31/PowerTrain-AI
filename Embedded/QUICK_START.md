# 🚀 Quick Start - ESP32 Development in Cursor

## Installation (One Time)

```powershell
# From project root
.\scripts\install_platformio.ps1
```

## Daily Workflow

### 1. Build (Compile)
```powershell
cd Embedded
pio run
```
**Or:** `Ctrl+Shift+B` in Cursor

### 2. Upload to ESP32
```powershell
pio run -t upload
```
**Or:** `Ctrl+Shift+P` → `Tasks: Run Task` → `PlatformIO: Upload`

### 3. Monitor Serial Output
```powershell
pio device monitor
```
**Or:** `Ctrl+Shift+P` → `Tasks: Run Task` → `PlatformIO: Monitor`

### 4. Upload + Monitor (All-in-One)
```powershell
pio run -t upload && pio device monitor
```
**Or:** `Ctrl+Shift+P` → `Tasks: Run Task` → `PlatformIO: Upload & Monitor`

## VS Code Tasks (Keyboard Shortcuts)

| Action | Shortcut | Task Name |
|-------|----------|-----------|
| Build | `Ctrl+Shift+B` | PlatformIO: Build |
| Upload | `Ctrl+Shift+P` → Tasks | PlatformIO: Upload |
| Monitor | `Ctrl+Shift+P` → Tasks | PlatformIO: Monitor |
| Upload+Monitor | `Ctrl+Shift+P` → Tasks | PlatformIO: Upload & Monitor |

## Troubleshooting

**"pio: command not found"**
- Restart terminal
- Or use: `python -m platformio`

**"Upload failed"**
- Check COM port: `pio device list`
- Hold BOOT button during upload

**"Library not found"**
- Run: `pio lib install`

## File Locations

- **Main code:** `Embedded/src/main.cpp`
- **Config:** `Embedded/src/config/config.h` (generate with `python scripts/generate_config.py`)
- **PlatformIO config:** `Embedded/platformio.ini`

## Next Steps

See `PLATFORMIO_SETUP.md` for complete documentation.

