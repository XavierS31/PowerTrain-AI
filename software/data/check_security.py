"""
Security Check Script
Scans for exposed sensitive information (API keys, passwords, IPs, etc.)
"""

import os
import re
from pathlib import Path

# Patterns to detect sensitive information
SENSITIVE_PATTERNS = {
    'ip_address': r'\b(?:[0-9]{1,3}\.){3}[0-9]{1,3}\b',
    'password': r'password\s*[=:]\s*["\']?[^"\'\s]+["\']?',
    'api_key': r'api[_-]?key\s*[=:]\s*["\']?[^"\'\s]+["\']?',
    'secret': r'secret\s*[=:]\s*["\']?[^"\'\s]+["\']?',
    'token': r'token\s*[=:]\s*["\']?[^"\'\s]+["\']?',
    'wifi_ssid': r'(?:wifi|ssid)\s*[=:]\s*["\']?[^"\'\s]+["\']?',
    'wifi_password': r'(?:wifi|wlan).*password\s*[=:]\s*["\']?[^"\'\s]+["\']?',
    'hardcoded_ip': r'["\'](?:192\.168|10\.|172\.(?:1[6-9]|2[0-9]|3[01]))\.[0-9]{1,3}\.[0-9]{1,3}["\']',
}

# Files to check
SEARCH_DIRS = ['Software', 'Embedded']
EXCLUDE_PATTERNS = ['.git', '__pycache__', '.pio', 'node_modules', '.venv', 'venv']

# Placeholder patterns (these are OK)
PLACEHOLDERS = [
    'YOUR_WIFI_SSID',
    'YOUR_WIFI_PASSWORD',
    'YOUR_IP',
    'SERVER_IP',
    '192.168.1.100',  # Example IP
    'localhost',
    '127.0.0.1',
]

def is_placeholder(value):
    """Check if a value is a placeholder"""
    value_lower = value.lower()
    for placeholder in PLACEHOLDERS:
        if placeholder.lower() in value_lower:
            return True
    return False

def scan_file(filepath):
    """Scan a file for sensitive information"""
    issues = []
    try:
        with open(filepath, 'r', encoding='utf-8', errors='ignore') as f:
            lines = f.readlines()
            
        for line_num, line in enumerate(lines, 1):
            line_lower = line.lower()
            
            for pattern_name, pattern in SENSITIVE_PATTERNS.items():
                matches = re.finditer(pattern, line, re.IGNORECASE)
                for match in matches:
                    value = match.group(0)
                    if not is_placeholder(value):
                        issues.append({
                            'file': filepath,
                            'line': line_num,
                            'type': pattern_name,
                            'content': line.strip()[:100]
                        })
    except Exception as e:
        pass
    
    return issues

def scan_directory(directory):
    """Recursively scan directory for sensitive information"""
    all_issues = []
    
    for root, dirs, files in os.walk(directory):
        # Skip excluded directories
        dirs[:] = [d for d in dirs if not any(exclude in d for exclude in EXCLUDE_PATTERNS)]
        
        for file in files:
            filepath = os.path.join(root, file)
            
            # Only check text files
            if file.endswith(('.py', '.cpp', '.h', '.ino', '.hpp', '.c', '.js', '.ts', '.json', '.md', '.txt', '.bat', '.sh')):
                issues = scan_file(filepath)
                all_issues.extend(issues)
    
    return all_issues

if __name__ == '__main__':
    print("=" * 60)
    print("Security Check - Scanning for Exposed Sensitive Information")
    print("=" * 60)
    
    project_root = os.path.abspath(os.path.join(os.path.dirname(__file__), '..', '..'))
    
    all_issues = []
    for search_dir in SEARCH_DIRS:
        dir_path = os.path.join(project_root, search_dir)
        if os.path.exists(dir_path):
            print(f"\nScanning {search_dir}/...")
            issues = scan_directory(dir_path)
            all_issues.extend(issues)
    
    # Also check root level files
    print(f"\nScanning root level files...")
    for file in os.listdir(project_root):
        filepath = os.path.join(project_root, file)
        if os.path.isfile(filepath) and file.endswith(('.py', '.md', '.txt', '.bat', '.sh')):
            issues = scan_file(filepath)
            all_issues.extend(issues)
    
    print("\n" + "=" * 60)
    print("Security Check Results")
    print("=" * 60)
    
    if all_issues:
        print(f"\nWARNING: Found {len(all_issues)} potential security issues:\n")
        
        for issue in all_issues:
            print(f"File: {issue['file']}")
            print(f"  Line {issue['line']}: {issue['type']}")
            print(f"  Content: {issue['content']}")
            print()
        
        print("\nRecommendations:")
        print("1. Move sensitive values to .env file")
        print("2. Add affected files to .gitignore")
        print("3. Use environment variables or config files (not in git)")
        print("4. Review and remove hardcoded credentials")
    else:
        print("\nNo exposed sensitive information found.")
        print("All detected values appear to be placeholders.")
    
    print("\n" + "=" * 60)

