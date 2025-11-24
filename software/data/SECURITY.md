# Security Guidelines

## Sensitive Information Protection

This document outlines security practices for protecting sensitive information in the PowerTrain AI project.

## Protected Files

The following files are excluded from version control via `.gitignore`:

- `.env` - Environment variables (passwords, API keys, IPs)
- `Embedded/src/config/config.h` - ESP32 configuration (WiFi credentials, server IPs)

## Configuration Files

### Environment Variables (.env)

Create a `.env` file in the project root with:

```
SERVER_PORT=5000
DATA_API_PORT=5001
WIFI_SSID=your_actual_ssid
WIFI_PASSWORD=your_actual_password
SERVER_IP=your_server_ip
```

### ESP32 Configuration (config.h)

The file `Embedded/src/config/config.h` contains WiFi credentials and server IPs. This file is in `.gitignore`.

Use `Embedded/src/config/config.h.template` as a reference for the structure.

## Security Check

Run the security check script to scan for exposed sensitive information:

```bash
cd Software/data
python check_security.py
```

This will identify:
- Hardcoded IP addresses
- Passwords in code
- API keys
- WiFi credentials
- Other sensitive data

## Best Practices

1. Never commit `.env` files
2. Never commit `config.h` with real credentials
3. Use placeholders in code (e.g., "YOUR_WIFI_SSID")
4. Use environment variables for sensitive values
5. Review code before committing
6. Run security check before pushing

## API Security

The data API (`data_api.py`) and data receiver (`data_receiver.py`) use environment variables for configuration. Ensure `.env` is properly configured and not committed.

## If Credentials Are Exposed

If sensitive information is accidentally committed:

1. Remove from git history: `git filter-branch` or BFG Repo-Cleaner
2. Rotate all exposed credentials immediately
3. Update `.gitignore` to prevent future commits
4. Review commit history for other exposures

