"""
Create SQLite database schema and import data from CSV
This script creates the database structure and loads data from driving_behavior_dataset.csv
"""

import pandas as pd
import sqlite3
import os

# Define paths
PROJECT_ROOT = os.path.abspath(os.path.join(os.path.dirname(__file__), '..', '..'))
DATA_DIR = os.path.join(PROJECT_ROOT, 'Software', 'data')
RAW_DATA_DIR = os.path.join(DATA_DIR, 'raw_data')
DB_DIR = os.path.join(DATA_DIR, 'database')
os.makedirs(DB_DIR, exist_ok=True)

DB_FILE = os.path.join(DB_DIR, 'driving_behavior.db')
CSV_FILE = os.path.join(RAW_DATA_DIR, 'driving_behavior_dataset.csv')

# SQLite schema for driving behavior data
SCHEMA = """
CREATE TABLE IF NOT EXISTS driving_behavior (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    Level INTEGER NOT NULL,
    Time REAL,
    InternalResistance REAL,
    StartingInternalResistance REAL,
    Voltage REAL,
    Current REAL,
    Temp REAL,
    AccelX REAL,
    AccelY REAL,
    AccelCombined REAL,
    Speed REAL,
    Distance REAL,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);

CREATE INDEX IF NOT EXISTS idx_level ON driving_behavior(Level);
CREATE INDEX IF NOT EXISTS idx_time ON driving_behavior(Time);
CREATE INDEX IF NOT EXISTS idx_voltage ON driving_behavior(Voltage);
"""

def create_database():
    """Create database with schema"""
    conn = sqlite3.connect(DB_FILE)
    cursor = conn.cursor()
    
    # Create table
    cursor.executescript(SCHEMA)
    conn.commit()
    
    print(f"Database created at: {DB_FILE}")
    print("Schema created successfully")
    
    conn.close()

def import_csv_to_database():
    """Import data from CSV file to SQLite database"""
    if not os.path.exists(CSV_FILE):
        print(f"Error: CSV file not found: {CSV_FILE}")
        print("Please ensure driving_behavior_dataset.csv exists in Software/data/raw_data/")
        return False
    
    print(f"Loading data from CSV: {CSV_FILE}")
    df = pd.read_csv(CSV_FILE)
    
    # Ensure all required columns exist
    required_cols = [
        'Level', 'Time', 'InternalResistance', 'StartingInternalResistance',
        'Voltage', 'Current', 'Temp', 'AccelX', 'AccelY', 'AccelCombined',
        'Speed', 'Distance'
    ]
    
    missing_cols = [col for col in required_cols if col not in df.columns]
    if missing_cols:
        print(f"Warning: Missing columns: {missing_cols}")
        print(f"Available columns: {list(df.columns)}")
    
    # Select only the columns we need
    available_cols = [col for col in required_cols if col in df.columns]
    df_selected = df[available_cols].copy()
    
    # Connect to database
    conn = sqlite3.connect(DB_FILE)
    
    # Clear existing data (optional - comment out if you want to append)
    cursor = conn.cursor()
    cursor.execute("DELETE FROM driving_behavior")
    conn.commit()
    
    # Insert data
    print(f"Importing {len(df_selected)} records to database...")
    df_selected.to_sql('driving_behavior', conn, if_exists='append', index=False)
    
    # Verify import
    cursor.execute("SELECT COUNT(*) FROM driving_behavior")
    count = cursor.fetchone()[0]
    
    conn.close()
    
    print(f"Successfully imported {count} records to database")
    return True

if __name__ == "__main__":
    print("=" * 60)
    print("Creating SQLite Database for Driving Behavior Data")
    print("=" * 60)
    
    # Create database
    print("\n1. Creating database schema...")
    create_database()
    
    # Import CSV data
    print("\n2. Importing data from CSV...")
    if import_csv_to_database():
        print("\n" + "=" * 60)
        print("Database setup complete!")
        print(f"Database location: {DB_FILE}")
        print("=" * 60)
    else:
        print("\nDatabase created but CSV import failed.")
        print("You can manually import data later or run preprocessing.py first.")

