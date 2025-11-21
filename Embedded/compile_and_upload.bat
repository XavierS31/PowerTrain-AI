@echo off
echo ========================================
echo   ESP32 Car - Compile and Upload
echo ========================================
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
echo [1/2] Compiling...
pio run

if %ERRORLEVEL% NEQ 0 (
    echo.
    echo ========================================
    echo   Compilation FAILED! Stopping.
    echo ========================================
    pause
    exit /b 1
)

echo.
echo [2/2] Uploading to ESP32...
pio run -t upload

if %ERRORLEVEL% EQU 0 (
    echo.
    echo ========================================
    echo   SUCCESS! Code uploaded to ESP32
    echo   You can now unplug and run standalone
    echo ========================================
) else (
    echo.
    echo ========================================
    echo   Upload FAILED!
    echo   Try putting ESP32 in download mode
    echo ========================================
)

pause

