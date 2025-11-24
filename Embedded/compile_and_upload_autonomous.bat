@echo off
title ESP32 - Compile and Upload Autonomous Driving
color 0A
echo.
echo AUTONOMOUS DRIVING
echo Compile and Upload to ESP32
echo.
echo This will compile and upload autonomous driving code
echo Supports both .ino and .cpp files
echo.
echo Make sure ESP32 is connected to COM4
echo If upload fails, put ESP32 in download mode:
echo   1. Hold BOOT button
echo   2. Press and release RESET button
echo   3. Release BOOT button
echo.
pause

cd /d "%~dp0"

echo.
echo [1/2] Compiling with PlatformIO...
pio run

if %ERRORLEVEL% NEQ 0 (
    echo.
    echo   Compilation FAILED! Stopping.
    pause
    exit /b 1
)

echo.
echo [2/2] Uploading to ESP32...
pio run -t upload

if %ERRORLEVEL% EQU 0 (
    echo.
    echo   SUCCESS! Autonomous driving code uploaded
    echo   Car will start driving autonomously
) else (
    echo.
    echo   Upload FAILED!
    echo   Try putting ESP32 in download mode
)

pause

