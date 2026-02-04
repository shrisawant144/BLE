# Security Policy

## Supported Versions

| Version | Supported          |
| ------- | ------------------ |
| 1.0.x   | :white_check_mark: |

## Reporting a Vulnerability

If you discover a security vulnerability, please email security@example.com.

**Please do not open public issues for security vulnerabilities.**

We will respond within 48 hours and work with you to address the issue.

## Security Considerations

This project involves Bluetooth Low Energy operations which require:

1. **Root privileges** - Many BLE operations require sudo
2. **Physical access** - BLE has limited range
3. **Pairing** - Some operations require device pairing
4. **Privacy** - BLE addresses can be tracked

### Best Practices

- Only run examples in controlled environments
- Review code before executing with sudo
- Use random MAC addresses when possible
- Implement proper authentication for production use
- Keep BlueZ and system packages updated
