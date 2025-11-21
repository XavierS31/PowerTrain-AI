@echo off
echo ========================================
echo   ESP32 Car - Serial Monitor
echo ========================================
echo.
echo Press Ctrl+C to exit monitor
echo.

cd /d "%~dp0"
pio device monitor

pause

