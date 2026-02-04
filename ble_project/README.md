# BLE Learning Project

[![CI](https://github.com/YOUR_USERNAME/ble_project/workflows/CI/badge.svg)](https://github.com/YOUR_USERNAME/ble_project/actions)
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)

Comprehensive BLE (Bluetooth Low Energy) learning project with working examples.

## Quick Start

```bash
./scripts/install_deps.sh  # Install dependencies
./setup_bluetooth.sh       # Setup Bluetooth
make build                 # Build all examples
cd build/bin && sudo ./ble_scan  # Run example
```

## Examples

### Central (Client)
- `ble_scan` - Discover nearby BLE devices
- `ble_connect` - Connect and discover services/characteristics
- `ble_rw` - Read and write characteristic values
- `ble_notify` - Subscribe to characteristic notifications

### Peripheral (Server)
- `simple_peripheral` - Basic GATT server with read/write
- `ble_peripheral_notify` - GATT server with notifications

## Documentation

See [docs/README.md](docs/README.md) for complete documentation.

## Build Options

```bash
make build      # Build all
make central    # Build only central
make peripheral # Build only peripheral
make clean      # Clean build
make docs       # Generate docs
```

## Requirements

- Linux with Bluetooth adapter
- BlueZ 5.x
- GCC/G++ and CMake 3.10+

## License

MIT License - See [LICENSE](LICENSE)
