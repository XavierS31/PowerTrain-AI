@echo off
echo ========================================
echo   ESP32 Car - Data Receiver Server
echo ========================================
echo.
echo Starting data receiver server...
echo Data will be saved to: raw_data/
echo.
echo Press Ctrl+C to stop the server
echo.

cd /d "%~dp0"
python data_receiver.py

pause

