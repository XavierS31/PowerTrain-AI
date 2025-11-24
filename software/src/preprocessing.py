"""
Preprocessing Module for Raw Sensor Data
Processes data from the data receiver in multiple formats (JSON, CSV, SQLite, Parquet, HDF5, Excel)
and converts it to a format suitable for ML training.

Based on the example: driving_behavior_dataset.csv
Output format: Level, Time, InternalResistance, StartingInternalResistance, Voltage, Current, Temp, 
               AccelX, AccelY, AccelCombined, Speed, Distance
"""

import pandas as pd
import numpy as np
import os
import json
import sqlite3
from datetime import datetime
from pathlib import Path
import warnings
warnings.filterwarnings('ignore')

# Configuration
# Input directory (where data receiver saves files)
RAW_DATA_DIR = os.path.join(os.path.dirname(__file__), '..', 'data', 'raw_data')

# Output directory for processed data
PROCESSED_DATA_DIR = os.path.join(os.path.dirname(__file__), '..', 'data', 'processed')

# Ensure output directory exists
os.makedirs(PROCESSED_DATA_DIR, exist_ok=True)

# Battery configuration
V_NOMINAL = 6.0  # Nominal battery voltage (adjust based on your battery)
STARTING_RINT = 0.6  # Starting internal resistance (ohms)

# Level assignment thresholds (based on performance)
# You can adjust these based on your requirements
LEVEL_THRESHOLDS = {
    10: {'rint_max': 0.1, 'voltage_min': 5.8, 'current_max': 0.5},  # Excellent
    9: {'rint_max': 0.2, 'voltage_min': 5.7, 'current_max': 0.6},
    8: {'rint_max': 0.3, 'voltage_min': 5.6, 'current_max': 0.7},
    7: {'rint_max': 0.4, 'voltage_min': 5.5, 'current_max': 0.8},
    6: {'rint_max': 0.5, 'voltage_min': 5.4, 'current_max': 0.9},
    5: {'rint_max': 0.6, 'voltage_min': 5.3, 'current_max': 1.0},
    4: {'rint_max': 0.7, 'voltage_min': 5.2, 'current_max': 1.1},
    3: {'rint_max': 0.8, 'voltage_min': 5.1, 'current_max': 1.2},
    2: {'rint_max': 0.9, 'voltage_min': 5.0, 'current_max': 1.3},
    1: {'rint_max': float('inf'), 'voltage_min': 0.0, 'current_max': float('inf')}  # Poor
}

# Data Loading Functions

def load_from_csv(filepath):
    """Load data from CSV file"""
    try:
        df = pd.read_csv(filepath)
        print(f"Loaded {len(df)} records from CSV: {filepath}")
        return df
    except Exception as e:
        print(f"Error loading CSV: {e}")
        return None

def load_from_json(filepath):
    """Load data from JSON Lines file"""
    try:
        data = []
        with open(filepath, 'r') as f:
            for line in f:
                if line.strip():
                    data.append(json.loads(line))
        df = pd.DataFrame(data)
        print(f"Loaded {len(df)} records from JSON: {filepath}")
        return df
    except Exception as e:
        print(f"Error loading JSON: {e}")
        return None

def load_from_sqlite(filepath):
    """Load data from SQLite database"""
    try:
        conn = sqlite3.connect(filepath)
        df = pd.read_sql_query("SELECT * FROM sensor_data", conn)
        conn.close()
        print(f"Loaded {len(df)} records from SQLite: {filepath}")
        return df
    except Exception as e:
        print(f"Error loading SQLite: {e}")
        return None

def load_from_parquet(filepath):
    """Load data from Parquet file"""
    try:
        df = pd.read_parquet(filepath)
        print(f"Loaded {len(df)} records from Parquet: {filepath}")
        return df
    except Exception as e:
        print(f"Error loading Parquet: {e}")
        return None

def load_from_hdf5(filepath):
    """Load data from HDF5 file"""
    try:
        df = pd.read_hdf(filepath, key='sensor_data')
        print(f"Loaded {len(df)} records from HDF5: {filepath}")
        return df
    except Exception as e:
        print(f"Error loading HDF5: {e}")
        return None

def load_from_excel(filepath):
    """Load data from Excel file"""
    try:
        df = pd.read_excel(filepath)
        print(f"Loaded {len(df)} records from Excel: {filepath}")
        return df
    except Exception as e:
        print(f"Error loading Excel: {e}")
        return None

def load_raw_data(filepath=None, format_type=None):
    """
    Load raw data from various formats
    
    Args:
        filepath: Path to the data file (if None, searches for latest file)
        format_type: 'csv', 'json', 'sqlite', 'parquet', 'hdf5', 'excel' (if None, auto-detect)
    
    Returns:
        DataFrame with raw sensor data
    """
    if filepath is None:
        # Find the most recent file in raw_data directory
        files = []
        for ext in ['*.csv', '*.jsonl', '*.db', '*.parquet', '*.h5', '*.hdf5', '*.xlsx']:
            files.extend(Path(RAW_DATA_DIR).glob(ext))
        
        if not files:
            print(f"No data files found in {RAW_DATA_DIR}")
            return None
        
        # Get most recent file
        filepath = max(files, key=os.path.getmtime)
        print(f"Auto-detected file: {filepath}")
    
    # Auto-detect format if not specified
    if format_type is None:
        ext = Path(filepath).suffix.lower()
        format_map = {
            '.csv': 'csv',
            '.jsonl': 'json',
            '.db': 'sqlite',
            '.parquet': 'parquet',
            '.h5': 'hdf5',
            '.hdf5': 'hdf5',
            '.xlsx': 'excel'
        }
        format_type = format_map.get(ext, 'csv')
    
    # Load based on format
    loaders = {
        'csv': load_from_csv,
        'json': load_from_json,
        'sqlite': load_from_sqlite,
        'parquet': load_from_parquet,
        'hdf5': load_from_hdf5,
        'excel': load_from_excel
    }
    
    loader = loaders.get(format_type.lower())
    if loader:
        return loader(filepath)
    else:
        print(f"Unsupported format: {format_type}")
        return None

# Data Processing Functions

def calculate_internal_resistance(voltage, current, v_nominal=V_NOMINAL):
    """
    Calculate internal resistance: Rint = (V_nominal - V_actual) / I
    If current is too small, use alternative: Rint = V / I
    """
    rint = np.zeros_like(voltage)
    
    # Method 1: Using voltage drop
    voltage_drop = v_nominal - voltage
    mask = current > 0.01  # Avoid division by zero
    
    rint[mask] = voltage_drop[mask] / current[mask]
    
    # If voltage drop is negative or very small, use direct calculation
    negative_mask = (rint < 0) | (voltage_drop < 0)
    rint[negative_mask & mask] = voltage[negative_mask & mask] / current[negative_mask & mask]
    
    # Handle zero current
    rint[~mask] = 0.0
    
    # Clip to reasonable range (0 to 2 ohms)
    rint = np.clip(rint, 0.0, 2.0)
    
    return rint

def calculate_accel_combined(accelX, accelY, accelZ):
    """Calculate combined acceleration magnitude"""
    return np.sqrt(accelX**2 + accelY**2 + accelZ**2)

def assign_level(rint, voltage, current):
    """
    Assign performance level (1-10) based on internal resistance, voltage, and current
    Higher level = better performance
    """
    levels = np.ones(len(rint), dtype=int)
    
    for level in range(10, 0, -1):
        threshold = LEVEL_THRESHOLDS[level]
        mask = (
            (rint <= threshold['rint_max']) &
            (voltage >= threshold['voltage_min']) &
            (current <= threshold['current_max'])
        )
        levels[mask] = level
    
    return levels

def process_raw_data(df):
    """
    Process raw sensor data into ML-ready format
    
    Args:
        df: DataFrame with raw sensor data from data receiver
    
    Returns:
        DataFrame with processed features
    """
    if df is None or df.empty:
        print("No data to process")
        return None
    
    print(f"\nProcessing {len(df)} records...")
    
    # Create a copy to avoid modifying original
    processed = df.copy()
    
    # Handle Missing Columns
    # Ensure all required columns exist
    required_cols = {
        'voltage': 0.0,
        'current': 0.0,
        'temperature': 25.0,
        'accelX': 0.0,
        'accelY': 0.0,
        'accelZ': 0.0,
        'speed': 0.0,
        'distance': 0.0
    }
    
    for col, default in required_cols.items():
        if col not in processed.columns:
            processed[col] = default
            print(f"Warning: Missing column '{col}', using default value {default}")
    
    # Calculate Features
    
    # Internal Resistance
    print("Calculating internal resistance...")
    processed['InternalResistance'] = calculate_internal_resistance(
        processed['voltage'].values,
        processed['current'].values,
        V_NOMINAL
    )
    
    # Starting Internal Resistance (use first value or constant)
    processed['StartingInternalResistance'] = STARTING_RINT
    
    # Combined Acceleration
    print("Calculating combined acceleration...")
    if 'acceleration' in processed.columns:
        # Use pre-calculated acceleration if available
        processed['AccelCombined'] = processed['acceleration']
    else:
        processed['AccelCombined'] = calculate_accel_combined(
            processed['accelX'].values,
            processed['accelY'].values,
            processed['accelZ'].values
        )
    
    # Time (relative to first timestamp)
    print("Calculating time...")
    if 'timestamp' in processed.columns:
        # Convert to relative time in seconds
        if processed['timestamp'].dtype == 'object':
            # If it's a string, try to parse it
            try:
                processed['timestamp'] = pd.to_datetime(processed['timestamp'])
            except:
                # If it's numeric, assume milliseconds
                processed['timestamp'] = pd.to_numeric(processed['timestamp'], errors='coerce')
        
        if processed['timestamp'].dtype in ['int64', 'float64']:
            # Assume milliseconds
            processed['Time'] = (processed['timestamp'] - processed['timestamp'].iloc[0]) / 1000.0
        else:
            # Datetime
            processed['Time'] = (processed['timestamp'] - processed['timestamp'].iloc[0]).dt.total_seconds()
    else:
        # Create sequential time
        processed['Time'] = range(len(processed))
    
    # Cumulative Distance
    print("Calculating cumulative distance...")
    if 'distance' in processed.columns:
        # If distance is already cumulative, use it
        # Otherwise, calculate cumulative sum
        if processed['distance'].iloc[-1] > processed['distance'].iloc[0]:
            processed['Distance'] = processed['distance']
        else:
            processed['Distance'] = processed['distance'].cumsum()
    else:
        # Estimate from speed
        if 'speed' in processed.columns:
            time_diff = processed['Time'].diff().fillna(0)
            processed['Distance'] = (processed['speed'] * time_diff).cumsum()
        else:
            processed['Distance'] = 0.0
    
    # Performance Level
    print("Assigning performance levels...")
    processed['Level'] = assign_level(
        processed['InternalResistance'].values,
        processed['voltage'].values,
        processed['current'].values
    )
    
    # Rename Columns
    column_mapping = {
        'voltage': 'Voltage',
        'current': 'Current',
        'temperature': 'Temp',
        'accelX': 'AccelX',
        'accelY': 'AccelY',
        'speed': 'Speed',
        'distance': 'Distance'
    }
    
    for old_col, new_col in column_mapping.items():
        if old_col in processed.columns:
            processed.rename(columns={old_col: new_col}, inplace=True)
    
    # Select Final Columns
    final_columns = [
        'Level', 'Time', 'InternalResistance', 'StartingInternalResistance',
        'Voltage', 'Current', 'Temp', 'AccelX', 'AccelY', 'AccelCombined',
        'Speed', 'Distance'
    ]
    
    # Only include columns that exist
    available_columns = [col for col in final_columns if col in processed.columns]
    processed = processed[available_columns]
    
    # Clean Data
    # Remove rows with invalid data
    initial_len = len(processed)
    processed = processed.dropna()
    processed = processed[(processed['Voltage'] > 0) & (processed['Current'] >= 0)]
    
    if len(processed) < initial_len:
        print(f"Removed {initial_len - len(processed)} rows with invalid data")
    
    print(f"\nProcessed {len(processed)} records successfully")
    print(f"Columns: {list(processed.columns)}")
    
    return processed

def save_processed_data(df, output_filename=None):
    """
    Save processed data to CSV
    
    Args:
        df: Processed DataFrame
        output_filename: Output filename (if None, auto-generate)
    """
    if df is None or df.empty:
        print("No data to save")
        return None
    
    if output_filename is None:
        timestamp = datetime.now().strftime('%Y%m%d_%H%M%S')
        output_filename = f"processed_data_{timestamp}.csv"
    
    output_path = os.path.join(PROCESSED_DATA_DIR, output_filename)
    df.to_csv(output_path, index=False)
    print(f"\nProcessed data saved to: {output_path}")
    return output_path

# Main Processing Function

def preprocess_data(input_file=None, input_format=None, output_file=None):
    """
    Main function to preprocess raw sensor data
    
    Args:
        input_file: Path to input file (None = auto-detect latest)
        input_format: 'csv', 'json', 'sqlite', 'parquet', 'hdf5', 'excel' (None = auto-detect)
        output_file: Output filename (None = auto-generate)
    
    Returns:
        Processed DataFrame
    """
    print("DATA PREPROCESSING")
    
    # Load raw data
    print("\n[1/3] Loading raw data...")
    raw_df = load_raw_data(input_file, input_format)
    
    if raw_df is None:
        print("Failed to load raw data")
        return None
    
    # Process data
    print("\n[2/3] Processing data...")
    processed_df = process_raw_data(raw_df)
    
    if processed_df is None:
        print("Failed to process data")
        return None
    
    # Save processed data
    print("\n[3/3] Saving processed data...")
    output_path = save_processed_data(processed_df, output_file)
    
    # Print summary statistics
    print("\nSUMMARY STATISTICS")
    print(processed_df.describe())
    print(f"\nLevel distribution:")
    print(processed_df['Level'].value_counts().sort_index())
    
    return processed_df

# Command Line Interface

if __name__ == '__main__':
    import argparse
    
    parser = argparse.ArgumentParser(description='Preprocess raw sensor data from data receiver')
    parser.add_argument('--input', '-i', type=str, help='Input file path (auto-detect if not specified)')
    parser.add_argument('--format', '-f', type=str, 
                       choices=['csv', 'json', 'sqlite', 'parquet', 'hdf5', 'excel'],
                       help='Input file format (auto-detect if not specified)')
    parser.add_argument('--output', '-o', type=str, help='Output file name (auto-generate if not specified)')
    
    args = parser.parse_args()
    
    # Run preprocessing
    processed_data = preprocess_data(
        input_file=args.input,
        input_format=args.format,
        output_file=args.output
    )
    
    if processed_data is not None:
        print("\n✓ Preprocessing completed successfully!")
    else:
        print("\n✗ Preprocessing failed!")

