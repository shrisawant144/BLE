# Peripheral Examples

All examples are in `build/bin/` after building.

## Usage

```bash
cd build/bin
sudo ./simple_peripheral      # Basic GATT server
sudo ./temperature_sensor     # Temperature with notifications
sudo ./battery_service        # Battery service
sudo ./nordic_uart_server     # Nordic UART server
```

## Examples

1. **simple_peripheral** - Basic GATT server with read/write
2. **temperature_sensor** - Simulated sensor with notifications
3. **battery_service** - Standard battery service (0x180F)
4. **nordic_uart_server** - Serial communication server
