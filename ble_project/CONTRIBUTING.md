# Contributing to BLE Learning Project

## Getting Started

1. Fork the repository
2. Clone your fork: `git clone https://github.com/YOUR_USERNAME/ble_project.git`
3. Create a branch: `git checkout -b feature/your-feature`

## Development Setup

```bash
# Install dependencies
./setup_bluetooth.sh

# Build project
make build

# Run examples
cd build/central/examples/01_scan
sudo ./ble_scan
```

## Code Style

- Use 4 spaces for indentation
- Follow existing naming conventions
- Add comments for complex logic
- Keep functions focused and small

## Submitting Changes

1. Ensure code builds without warnings
2. Test your changes thoroughly
3. Update documentation if needed
4. Commit with clear messages
5. Push and create a pull request

## Adding Examples

New examples should include:
- Source code with comments
- CMakeLists.txt
- README.md explaining the example
- Pairing information (if applicable)
