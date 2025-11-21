"""
Generate config.h from .env file for Arduino/ESP32
Run this script from the project root directory
"""

import os
import sys
from pathlib import Path

# Get project root directory (parent of scripts folder)
project_root = Path(__file__).parent.parent
env_file = project_root / '.env'
config_file = project_root / 'Embedded' / 'src' / 'config' / 'config.h'

def read_env_file():
    """Read .env file and return dictionary of key-value pairs"""
    config = {}
    if not env_file.exists():
        print(f"Warning: .env file not found at {env_file}")
        print("Please create .env file from .env.example")
        return config
    
    with open(env_file, 'r') as f:
        for line in f:
            line = line.strip()
            # Skip comments and empty lines
            if not line or line.startswith('#'):
                continue
            # Parse key=value
            if '=' in line:
                key, value = line.split('=', 1)
                config[key.strip()] = value.strip()
    
    return config

def generate_config_header(config):
    """Generate config.h file from .env values"""
    header_content = '''/*
 * Configuration Header File
 * 
 * AUTO-GENERATED from .env file
 * DO NOT manually edit - run 'python scripts/generate_config.py' to regenerate
 * 
 * DO NOT commit sensitive data to version control
 */

#ifndef CONFIG_H
#define CONFIG_H

// ========== WiFi Configuration ==========
#define WIFI_SSID "{wifi_ssid}"
#define WIFI_PASSWORD "{wifi_password}"

// ========== Server Configuration ==========
#define SERVER_IP "{server_ip}"
#define SERVER_PORT {server_port}

// ========== Motor Speed Levels (PWM 0-255) ==========
#define MOTOR_SPEED_LOW {motor_speed_low}
#define MOTOR_SPEED_MEDIUM {motor_speed_medium}
#define MOTOR_SPEED_HIGH {motor_speed_high}

// ========== Obstacle Detection ==========
#define OBSTACLE_DISTANCE_CM {obstacle_distance_cm}
#define TURN_DURATION_MS {turn_duration_ms}

// ========== Sensor Reading Interval (milliseconds) ==========
#define SENSOR_READ_INTERVAL_MS {sensor_read_interval_ms}

// ========== TFT Update Interval (milliseconds) ==========
#define TFT_UPDATE_INTERVAL_MS {tft_update_interval_ms}

#endif // CONFIG_H
'''.format(
        wifi_ssid=config.get('WIFI_SSID', 'YOUR_WIFI_SSID'),
        wifi_password=config.get('WIFI_PASSWORD', 'YOUR_WIFI_PASSWORD'),
        server_ip=config.get('SERVER_IP', '192.168.1.100'),
        server_port=config.get('SERVER_PORT', '5000'),
        motor_speed_low=config.get('MOTOR_SPEED_LOW', '85'),
        motor_speed_medium=config.get('MOTOR_SPEED_MEDIUM', '170'),
        motor_speed_high=config.get('MOTOR_SPEED_HIGH', '255'),
        obstacle_distance_cm=config.get('OBSTACLE_DISTANCE_CM', '30'),
        turn_duration_ms=config.get('TURN_DURATION_MS', '500'),
        sensor_read_interval_ms=config.get('SENSOR_READ_INTERVAL_MS', '200'),
        tft_update_interval_ms=config.get('TFT_UPDATE_INTERVAL_MS', '500')
    )
    
    # Ensure config directory exists
    config_file.parent.mkdir(parents=True, exist_ok=True)
    
    # Write config file
    with open(config_file, 'w') as f:
        f.write(header_content)
    
    print(f"✓ Generated {config_file}")
    print(f"  WiFi SSID: {config.get('WIFI_SSID', 'NOT SET')}")
    print(f"  Server IP: {config.get('SERVER_IP', 'NOT SET')}")

if __name__ == '__main__':
    print("Generating config.h from .env file...")
    config = read_env_file()
    
    if config:
        generate_config_header(config)
        print("\n✓ Configuration file generated successfully!")
        print("  You can now compile and upload your ESP32 code.")
    else:
        print("\n✗ Failed to generate config file.")
        print("  Please create .env file first.")
        sys.exit(1)

