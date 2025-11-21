#!/bin/bash
# PlatformIO Installation Script for Linux/Mac
# Run this script to install PlatformIO Core

echo "========================================"
echo "PlatformIO Installation for ESP32"
echo "========================================"
echo ""

# Check if Python is installed
echo "Checking Python installation..."
if ! command -v python3 &> /dev/null; then
    echo "ERROR: Python 3 is not installed!"
    echo "Please install Python 3.7+ first"
    exit 1
fi

PYTHON_VERSION=$(python3 --version)
echo "✓ Python found: $PYTHON_VERSION"

# Install PlatformIO Core
echo ""
echo "Installing PlatformIO Core..."
python3 -m pip install --upgrade pip
python3 -m pip install platformio

if [ $? -ne 0 ]; then
    echo "ERROR: Failed to install PlatformIO!"
    exit 1
fi

echo "✓ PlatformIO installed successfully!"

# Verify installation
echo ""
echo "Verifying installation..."
if command -v pio &> /dev/null; then
    PIO_VERSION=$(pio --version)
    echo "✓ PlatformIO version: $PIO_VERSION"
else
    echo "ERROR: PlatformIO not found in PATH!"
    echo "You may need to add ~/.platformio/penv/bin to your PATH"
    echo "Add this to your ~/.bashrc or ~/.zshrc:"
    echo "export PATH=\$PATH:~/.platformio/penv/bin"
    exit 1
fi

# Install ESP32 platform
echo ""
echo "Installing ESP32 platform..."
pio platform install espressif32

if [ $? -ne 0 ]; then
    echo "WARNING: Failed to install ESP32 platform automatically"
    echo "You can install it manually later with: pio platform install espressif32"
else
    echo "✓ ESP32 platform installed!"
fi

echo ""
echo "========================================"
echo "Installation Complete!"
echo "========================================"
echo ""
echo "Next steps:"
echo "1. Navigate to Embedded directory: cd Embedded"
echo "2. Build project: pio run"
echo "3. Upload to ESP32: pio run -t upload"
echo "4. Monitor serial: pio device monitor"
echo ""

