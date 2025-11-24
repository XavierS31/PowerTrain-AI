"""
Data API Server
RESTful API to query and retrieve driving behavior data from SQLite database
"""

from flask import Flask, request, jsonify
from dotenv import load_dotenv
import sqlite3
import os
import pandas as pd
from datetime import datetime

# Load environment variables
project_root = os.path.abspath(os.path.join(os.path.dirname(__file__), '..', '..'))
env_path = os.path.join(project_root, '.env')
load_dotenv(env_path)

app = Flask(__name__)

# Enable CORS if flask-cors is available, otherwise add basic headers
try:
    from flask_cors import CORS
    CORS(app)
except ImportError:
    @app.after_request
    def after_request(response):
        response.headers.add('Access-Control-Allow-Origin', '*')
        response.headers.add('Access-Control-Allow-Headers', 'Content-Type,Authorization')
        response.headers.add('Access-Control-Allow-Methods', 'GET,POST,PUT,DELETE,OPTIONS')
        return response

# Configuration from environment variables
SERVER_PORT = int(os.getenv('DATA_API_PORT', '5001'))
DB_FILE = os.path.join(os.path.dirname(__file__), 'database', 'driving_behavior.db')

def get_db_connection():
    """Get SQLite database connection"""
    if not os.path.exists(DB_FILE):
        return None
    return sqlite3.connect(DB_FILE)

@app.route('/api/data/records', methods=['GET'])
def get_records():
    """Get driving behavior records with optional filtering"""
    try:
        conn = get_db_connection()
        if conn is None:
            return jsonify({'error': 'Database not found'}), 404
        
        # Query parameters
        limit = request.args.get('limit', default=100, type=int)
        offset = request.args.get('offset', default=0, type=int)
        level = request.args.get('level', type=int)
        min_voltage = request.args.get('min_voltage', type=float)
        max_voltage = request.args.get('max_voltage', type=float)
        
        # Build query
        query = "SELECT * FROM driving_behavior WHERE 1=1"
        params = []
        
        if level:
            query += " AND Level = ?"
            params.append(level)
        
        if min_voltage:
            query += " AND Voltage >= ?"
            params.append(min_voltage)
        
        if max_voltage:
            query += " AND Voltage <= ?"
            params.append(max_voltage)
        
        query += " ORDER BY id DESC LIMIT ? OFFSET ?"
        params.extend([limit, offset])
        
        df = pd.read_sql_query(query, conn, params=params)
        conn.close()
        
        return jsonify({
            'count': len(df),
            'data': df.to_dict('records')
        }), 200
        
    except Exception as e:
        return jsonify({'error': str(e)}), 500

@app.route('/api/data/stats', methods=['GET'])
def get_stats():
    """Get statistical summary of the data"""
    try:
        conn = get_db_connection()
        if conn is None:
            return jsonify({'error': 'Database not found'}), 404
        
        stats_query = """
        SELECT 
            COUNT(*) as total_records,
            COUNT(DISTINCT Level) as unique_levels,
            AVG(Voltage) as avg_voltage,
            AVG(Current) as avg_current,
            AVG(Temp) as avg_temp,
            AVG(InternalResistance) as avg_rint,
            MIN(Time) as min_time,
            MAX(Time) as max_time
        FROM driving_behavior
        """
        
        df = pd.read_sql_query(stats_query, conn)
        conn.close()
        
        return jsonify(df.to_dict('records')[0]), 200
        
    except Exception as e:
        return jsonify({'error': str(e)}), 500

@app.route('/api/data/levels', methods=['GET'])
def get_level_distribution():
    """Get distribution of performance levels"""
    try:
        conn = get_db_connection()
        if conn is None:
            return jsonify({'error': 'Database not found'}), 404
        
        query = """
        SELECT 
            Level,
            COUNT(*) as count,
            AVG(Voltage) as avg_voltage,
            AVG(Current) as avg_current,
            AVG(InternalResistance) as avg_rint
        FROM driving_behavior
        GROUP BY Level
        ORDER BY Level
        """
        
        df = pd.read_sql_query(query, conn)
        conn.close()
        
        return jsonify(df.to_dict('records')), 200
        
    except Exception as e:
        return jsonify({'error': str(e)}), 500

@app.route('/api/data/record/<int:record_id>', methods=['GET'])
def get_record_by_id(record_id):
    """Get a specific record by ID"""
    try:
        conn = get_db_connection()
        if conn is None:
            return jsonify({'error': 'Database not found'}), 404
        
        query = "SELECT * FROM driving_behavior WHERE id = ?"
        df = pd.read_sql_query(query, conn, params=[record_id])
        conn.close()
        
        if len(df) == 0:
            return jsonify({'error': 'Record not found'}), 404
        
        return jsonify(df.to_dict('records')[0]), 200
        
    except Exception as e:
        return jsonify({'error': str(e)}), 500

@app.route('/api/data/export', methods=['GET'])
def export_data():
    """Export data as CSV"""
    try:
        conn = get_db_connection()
        if conn is None:
            return jsonify({'error': 'Database not found'}), 404
        
        level = request.args.get('level', type=int)
        
        query = "SELECT * FROM driving_behavior WHERE 1=1"
        params = []
        
        if level:
            query += " AND Level = ?"
            params.append(level)
        
        df = pd.read_sql_query(query, conn, params=params)
        conn.close()
        
        csv_data = df.to_csv(index=False)
        
        from flask import Response
        return Response(
            csv_data,
            mimetype='text/csv',
            headers={'Content-Disposition': 'attachment; filename=driving_behavior_export.csv'}
        )
        
    except Exception as e:
        return jsonify({'error': str(e)}), 500

@app.route('/api/data/status', methods=['GET'])
def get_status():
    """Get API and database status"""
    db_exists = os.path.exists(DB_FILE)
    db_size = 0
    record_count = 0
    
    if db_exists:
        db_size = os.path.getsize(DB_FILE)
        try:
            conn = get_db_connection()
            if conn:
                cursor = conn.cursor()
                cursor.execute("SELECT COUNT(*) FROM driving_behavior")
                record_count = cursor.fetchone()[0]
                conn.close()
        except:
            pass
    
    return jsonify({
        'status': 'running',
        'database_exists': db_exists,
        'database_size_bytes': db_size,
        'record_count': record_count,
        'endpoints': {
            'records': '/api/data/records',
            'stats': '/api/data/stats',
            'levels': '/api/data/levels',
            'record_by_id': '/api/data/record/<id>',
            'export': '/api/data/export'
        }
    }), 200

if __name__ == '__main__':
    print("=" * 60)
    print("Data API Server")
    print("=" * 60)
    print(f"Database: {DB_FILE}")
    print(f"Server running on port: {SERVER_PORT}")
    print("\nAvailable endpoints:")
    print("  GET /api/data/records - Get records with filtering")
    print("  GET /api/data/stats - Get statistical summary")
    print("  GET /api/data/levels - Get level distribution")
    print("  GET /api/data/record/<id> - Get record by ID")
    print("  GET /api/data/export - Export data as CSV")
    print("  GET /api/data/status - Get API status")
    print("\nQuery parameters:")
    print("  ?limit=100 - Limit number of records")
    print("  ?offset=0 - Offset for pagination")
    print("  ?level=5 - Filter by performance level")
    print("  ?min_voltage=5.0 - Minimum voltage filter")
    print("  ?max_voltage=6.0 - Maximum voltage filter")
    print("=" * 60)
    
    app.run(host='0.0.0.0', port=SERVER_PORT, debug=False)

