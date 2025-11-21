# PlatformIO Installation Script for Windows
# Run this script to install PlatformIO Core

Write-Host "========================================" -ForegroundColor Cyan
Write-Host "PlatformIO Installation for ESP32" -ForegroundColor Cyan
Write-Host "========================================" -ForegroundColor Cyan
Write-Host ""

# Check if Python is installed
Write-Host "Checking Python installation..." -ForegroundColor Yellow
$pythonVersion = python --version 2>&1
if ($LASTEXITCODE -ne 0) {
    Write-Host "ERROR: Python is not installed!" -ForegroundColor Red
    Write-Host "Please install Python 3.7+ from https://www.python.org/downloads/" -ForegroundColor Red
    exit 1
}
Write-Host "✓ Python found: $pythonVersion" -ForegroundColor Green

# Install PlatformIO Core
Write-Host ""
Write-Host "Installing PlatformIO Core..." -ForegroundColor Yellow
python -m pip install --upgrade pip
python -m pip install platformio

if ($LASTEXITCODE -ne 0) {
    Write-Host "ERROR: Failed to install PlatformIO!" -ForegroundColor Red
    exit 1
}

Write-Host "✓ PlatformIO installed successfully!" -ForegroundColor Green

# Verify installation
Write-Host ""
Write-Host "Verifying installation..." -ForegroundColor Yellow
$pioVersion = pio --version 2>&1
if ($LASTEXITCODE -eq 0) {
    Write-Host "✓ PlatformIO version: $pioVersion" -ForegroundColor Green
} else {
    Write-Host "ERROR: PlatformIO not found in PATH!" -ForegroundColor Red
    Write-Host "You may need to restart your terminal or add Python Scripts to PATH" -ForegroundColor Yellow
    exit 1
}

# Install ESP32 platform
Write-Host ""
Write-Host "Installing ESP32 platform..." -ForegroundColor Yellow
pio platform install espressif32

if ($LASTEXITCODE -ne 0) {
    Write-Host "WARNING: Failed to install ESP32 platform automatically" -ForegroundColor Yellow
    Write-Host "You can install it manually later with: pio platform install espressif32" -ForegroundColor Yellow
} else {
    Write-Host "✓ ESP32 platform installed!" -ForegroundColor Green
}

Write-Host ""
Write-Host "========================================" -ForegroundColor Cyan
Write-Host "Installation Complete!" -ForegroundColor Green
Write-Host "========================================" -ForegroundColor Cyan
Write-Host ""
Write-Host "Next steps:" -ForegroundColor Yellow
Write-Host "1. Navigate to Embedded directory: cd Embedded" -ForegroundColor White
Write-Host "2. Build project: pio run" -ForegroundColor White
Write-Host "3. Upload to ESP32: pio run -t upload" -ForegroundColor White
Write-Host "4. Monitor serial: pio device monitor" -ForegroundColor White
Write-Host ""

