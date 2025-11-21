@echo off
title ESP32 Car - Master Control
color 0A
echo.
echo ========================================
echo   ESP32 CAR - MASTER CONTROL
echo ========================================
echo.
echo This will:
echo   1. Compile ESP32 code
echo   2. Upload to ESP32 (you'll need to put it in download mode)
echo   3. Optionally start data receiver server
echo.
echo ========================================
echo.

REM Check if ESP32 is connected
echo [Step 1] Checking ESP32 connection...
cd /d "%~dp0Embedded"
pio device list | findstr "COM4" >nul
if %ERRORLEVEL% NEQ 0 (
    echo WARNING: ESP32 not detected on COM4
    echo Make sure ESP32 is connected via USB
    echo.
    pause
)

REM Compile
echo.
echo ========================================
echo [Step 2] COMPILING CODE...
echo ========================================
echo.
pio run
if %ERRORLEVEL% NEQ 0 (
    echo.
    echo ========================================
    echo   COMPILATION FAILED!
    echo ========================================
    pause
    exit /b 1
)

echo.
echo ========================================
echo   COMPILATION SUCCESSFUL!
echo ========================================
echo.

REM Ask about data receiver
echo.
echo ========================================
echo [Step 3] DATA RECEIVER SERVER
echo ========================================
echo.
set /p start_receiver="Start data receiver server? (y/n): "
if /i "%start_receiver%"=="y" (
    echo.
    echo Starting data receiver in new window...
    start "ESP32 Data Receiver" cmd /k "cd /d %~dp0Software\data && python data_receiver.py"
    timeout /t 2 /nobreak >nul
    echo Data receiver started!
)

REM Upload
echo.
echo ========================================
echo [Step 4] UPLOADING TO ESP32
echo ========================================
echo.
echo IMPORTANT: Put ESP32 in download mode NOW:
echo   1. Hold BOOT button (keep holding)
echo   2. Press and release RESET button
echo   3. Release BOOT button
echo.
echo Then press any key to start upload...
pause

echo.
echo Uploading to ESP32...
pio run -t upload

if %ERRORLEVEL% EQU 0 (
    echo.
    echo ========================================
    echo   UPLOAD SUCCESSFUL!
    echo ========================================
    echo.
    echo ESP32 is ready!
    echo.
    echo What happens next:
    echo   - Press RESET button on ESP32 to restart
    echo   - Car will start driving automatically
    echo   - You can unplug USB and run standalone
    echo   - Data will be saved if receiver is running
    echo.
) else (
    echo.
    echo ========================================
    echo   UPLOAD FAILED!
    echo ========================================
    echo.
    echo Try again:
    echo   1. Put ESP32 in download mode:
    echo      - Hold BOOT button
    echo      - Press and release RESET
    echo      - Release BOOT button
    echo   2. Run this batch file again
    echo.
)

echo.
echo ========================================
echo   SETUP COMPLETE
echo ========================================
echo.
if /i "%start_receiver%"=="y" (
    echo Data receiver is running in another window
    echo Data will be saved to: Software\data\raw_data\
    echo.
)
echo You can now:
echo   - Unplug ESP32 and run standalone
echo   - Or keep connected to see Serial Monitor
echo.
echo To see Serial Monitor, run: Embedded\monitor.bat
echo.
pause

