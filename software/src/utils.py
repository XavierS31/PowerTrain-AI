"""
Utility functions: filters, helpers, constants
"""

# Constants
DEFAULT_BAUDRATE = 115200
DEFAULT_SERIAL_TIMEOUT = 1.0

# Sensor constants
VOLTAGE_RANGE = (0, 25)  # 0-25V range
CURRENT_RANGE = (-3.2, 3.2)  # INA219 range
TEMPERATURE_RANGE = (-55, 125)  # DS18B20 range


def apply_filter(data, filter_type='moving_average', window_size=10):
    """
    Apply filtering to sensor data
    
    Args:
        data: Input data
        filter_type: Type of filter ('moving_average', 'median', etc.)
        window_size: Window size for filter
    
    Returns:
        Filtered data
    """
    pass


def validate_sensor_data(data):
    """
    Validate sensor data ranges
    
    Args:
        data: Sensor data dictionary
    
    Returns:
        Boolean indicating if data is valid
    """
    pass

