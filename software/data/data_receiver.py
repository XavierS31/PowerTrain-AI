"""
Data Receiver Server for ESP32 Autonomous Car
Receives sensor data via WiFi and saves to software/data/raw/

Supported formats:
- JSON/JSONL (default) - Human readable, easy to parse
- CSV - Spreadsheet compatible
- Parquet - Efficient, compressed, great for ML (requires pyarrow)
- SQLite - Database format (built-in)
- HDF5 - Scientific data format (requires h5py)
- Excel - .xlsx files (requires openpyxl)
"""

from flask import Flask, request, jsonify
from datetime import datetime
from dotenv import load_dotenv
import json
import os
import csv
import sqlite3

# Load environment variables from .env file
# Look for .env in project root (2 levels up from this file)
project_root = os.path.abspath(os.path.join(os.path.dirname(__file__), '..', '..'))
env_path = os.path.join(project_root, '.env')
load_dotenv(env_path)

app = Flask(__name__)

# ========== CONFIGURATION (from .env file) ==========
# Choose which formats to save (set to True/False in .env file)
SAVE_JSON = os.getenv('SAVE_JSON', 'true').lower() == 'true'
SAVE_CSV = os.getenv('SAVE_CSV', 'false').lower() == 'true'
SAVE_SQLITE = os.getenv('SAVE_SQLITE', 'false').lower() == 'true'
SAVE_PARQUET = os.getenv('SAVE_PARQUET', 'false').lower() == 'true'
SAVE_HDF5 = os.getenv('SAVE_HDF5', 'false').lower() == 'true'
SAVE_EXCEL = os.getenv('SAVE_EXCEL', 'false').lower() == 'true'

# Server port from .env
SERVER_PORT = int(os.getenv('SERVER_PORT', '5000'))

# Data directory
DATA_DIR = os.path.join(os.path.dirname(__file__), 'raw')

# Ensure data directory exists
os.makedirs(DATA_DIR, exist_ok=True)

# File paths
today = datetime.now().strftime('%Y%m%d')
JSON_FILE = os.path.join(DATA_DIR, f"sensor_data_{today}.jsonl")
CSV_FILE = os.path.join(DATA_DIR, f"sensor_data_{today}.csv")
SQLITE_FILE = os.path.join(DATA_DIR, f"sensor_data_{today}.db")
PARQUET_FILE = os.path.join(DATA_DIR, f"sensor_data_{today}.parquet")
HDF5_FILE = os.path.join(DATA_DIR, f"sensor_data_{today}.h5")
EXCEL_FILE = os.path.join(DATA_DIR, f"sensor_data_{today}.xlsx")

# Initialize CSV file with headers if enabled
if SAVE_CSV and not os.path.exists(CSV_FILE):
    with open(CSV_FILE, 'w', newline='') as f:
        writer = csv.writer(f)
        writer.writerow([
            'timestamp', 'temperature', 'voltage', 'current', 'power',
            'accelX', 'accelY', 'accelZ', 'gyroX', 'gyroY', 'gyroZ',
            'distance', 'irDigital', 'irAnalog', 'received_at'
        ])

# Initialize SQLite database if enabled
if SAVE_SQLITE:
    conn = sqlite3.connect(SQLITE_FILE)
    cursor = conn.cursor()
    cursor.execute('''
        CREATE TABLE IF NOT EXISTS sensor_data (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            timestamp INTEGER,
            temperature REAL,
            voltage REAL,
            current REAL,
            power REAL,
            accelX REAL, accelY REAL, accelZ REAL,
            gyroX REAL, gyroY REAL, gyroZ REAL,
            distance REAL,
            irDigital INTEGER,
            irAnalog INTEGER,
            received_at TEXT
        )
    ''')
    conn.commit()
    conn.close()

# Initialize Parquet (will be created on first write if enabled)
parquet_buffer = []

# Initialize HDF5 (will be created on first write if enabled)
hdf5_initialized = False

# Initialize Excel (will be created on first write if enabled)
excel_buffer = []

def save_to_json(data):
    """Save data to JSON Lines format"""
    if SAVE_JSON:
        with open(JSON_FILE, 'a') as f:
            f.write(json.dumps(data) + '\n')

def save_to_csv(data):
    """Save data to CSV format"""
    if SAVE_CSV:
        with open(CSV_FILE, 'a', newline='') as f:
            writer = csv.writer(f)
            writer.writerow([
                data.get('timestamp', ''),
                data.get('temperature', 0),
                data.get('voltage', 0),
                data.get('current', 0),
                data.get('power', 0),
                data.get('accelX', 0),
                data.get('accelY', 0),
                data.get('accelZ', 0),
                data.get('gyroX', 0),
                data.get('gyroY', 0),
                data.get('gyroZ', 0),
                data.get('distance', 0),
                data.get('irDigital', 0),
                data.get('irAnalog', 0),
                data.get('received_at', '')
            ])

def save_to_sqlite(data):
    """Save data to SQLite database"""
    if SAVE_SQLITE:
        conn = sqlite3.connect(SQLITE_FILE)
        cursor = conn.cursor()
        cursor.execute('''
            INSERT INTO sensor_data 
            (timestamp, temperature, voltage, current, power,
             accelX, accelY, accelZ, gyroX, gyroY, gyroZ,
             distance, irDigital, irAnalog, received_at)
            VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)
        ''', (
            data.get('timestamp', 0),
            data.get('temperature', 0),
            data.get('voltage', 0),
            data.get('current', 0),
            data.get('power', 0),
            data.get('accelX', 0),
            data.get('accelY', 0),
            data.get('accelZ', 0),
            data.get('gyroX', 0),
            data.get('gyroY', 0),
            data.get('gyroZ', 0),
            data.get('distance', 0),
            data.get('irDigital', 0),
            data.get('irAnalog', 0),
            data.get('received_at', '')
        ))
        conn.commit()
        conn.close()

def save_to_parquet(data):
    """Save data to Parquet format (buffered, writes in batches)"""
    if SAVE_PARQUET:
        try:
            import pandas as pd
            global parquet_buffer
            
            parquet_buffer.append(data)
            
            # Write to parquet every 100 records (or on shutdown)
            if len(parquet_buffer) >= 100:
                df = pd.DataFrame(parquet_buffer)
                df.to_parquet(PARQUET_FILE, engine='pyarrow', append=True)
                parquet_buffer = []
        except ImportError:
            print("Warning: pyarrow not installed. Install with: pip install pyarrow")
        except Exception as e:
            print(f"Error saving to Parquet: {e}")

def save_to_hdf5(data):
    """Save data to HDF5 format"""
    if SAVE_HDF5:
        try:
            import h5py
            import numpy as np
            global hdf5_initialized
            
            with h5py.File(HDF5_FILE, 'a') as f:
                if 'sensor_data' not in f:
                    # Create dataset on first write
                    f.create_dataset('sensor_data', 
                                   maxshape=(None,),
                                   dtype=[('timestamp', 'i8'),
                                         ('temperature', 'f4'),
                                         ('voltage', 'f4'),
                                         ('current', 'f4'),
                                         ('power', 'f4'),
                                         ('accelX', 'f4'), ('accelY', 'f4'), ('accelZ', 'f4'),
                                         ('gyroX', 'f4'), ('gyroY', 'f4'), ('gyroZ', 'f4'),
                                         ('distance', 'f4'),
                                         ('irDigital', 'i4'),
                                         ('irAnalog', 'i4')])
                
                dataset = f['sensor_data']
                dataset.resize((len(dataset) + 1,))
                dataset[-1] = (
                    data.get('timestamp', 0),
                    data.get('temperature', 0),
                    data.get('voltage', 0),
                    data.get('current', 0),
                    data.get('power', 0),
                    data.get('accelX', 0), data.get('accelY', 0), data.get('accelZ', 0),
                    data.get('gyroX', 0), data.get('gyroY', 0), data.get('gyroZ', 0),
                    data.get('distance', 0),
                    data.get('irDigital', 0),
                    data.get('irAnalog', 0)
                )
        except ImportError:
            print("Warning: h5py not installed. Install with: pip install h5py")
        except Exception as e:
            print(f"Error saving to HDF5: {e}")

def save_to_excel(data):
    """Save data to Excel format (buffered, writes in batches)"""
    if SAVE_EXCEL:
        try:
            import pandas as pd
            global excel_buffer
            
            excel_buffer.append(data)
            
            # Write to Excel every 100 records
            if len(excel_buffer) >= 100:
                df = pd.DataFrame(excel_buffer)
                if os.path.exists(EXCEL_FILE):
                    # Append to existing file
                    existing_df = pd.read_excel(EXCEL_FILE)
                    df = pd.concat([existing_df, df], ignore_index=True)
                df.to_excel(EXCEL_FILE, index=False, engine='openpyxl')
                excel_buffer = []
        except ImportError:
            print("Warning: openpyxl not installed. Install with: pip install openpyxl")
        except Exception as e:
            print(f"Error saving to Excel: {e}")

@app.route('/api/data', methods=['POST'])
def receive_data():
    """Receive sensor data from ESP32"""
    try:
        data = request.get_json()
        
        # Add received timestamp
        data['received_at'] = datetime.now().isoformat()
        
        # Save to all enabled formats
        save_to_json(data)
        save_to_csv(data)
        save_to_sqlite(data)
        save_to_parquet(data)
        save_to_hdf5(data)
        save_to_excel(data)
        
        return jsonify({'status': 'success'}), 200
        
    except Exception as e:
        print(f"Error receiving data: {e}")
        return jsonify({'status': 'error', 'message': str(e)}), 500

@app.route('/api/status', methods=['GET'])
def status():
    """Check server status"""
    return jsonify({
        'status': 'running',
        'data_dir': DATA_DIR,
        'formats_enabled': {
            'json': SAVE_JSON,
            'csv': SAVE_CSV,
            'sqlite': SAVE_SQLITE,
            'parquet': SAVE_PARQUET,
            'hdf5': SAVE_HDF5,
            'excel': SAVE_EXCEL
        }
    }), 200

if __name__ == '__main__':
    print(f"Data receiver server starting...")
    print(f"Data will be saved to: {DATA_DIR}")
    print(f"\nEnabled formats:")
    if SAVE_JSON: print(f"  ✓ JSON: {JSON_FILE}")
    if SAVE_CSV: print(f"  ✓ CSV: {CSV_FILE}")
    if SAVE_SQLITE: print(f"  ✓ SQLite: {SQLITE_FILE}")
    if SAVE_PARQUET: print(f"  ✓ Parquet: {PARQUET_FILE}")
    if SAVE_HDF5: print(f"  ✓ HDF5: {HDF5_FILE}")
    if SAVE_EXCEL: print(f"  ✓ Excel: {EXCEL_FILE}")
    print("\nTo find your computer's IP address:")
    print("  Windows: ipconfig")
    print("  Linux/Mac: ifconfig or ip addr")
    print("\nUpdate .env file with your IP address and run:")
    print("  python Software/data/data_receiver.py")
    print(f"\nServer will run on http://0.0.0.0:{SERVER_PORT}")
    
    app.run(host='0.0.0.0', port=SERVER_PORT, debug=True)
