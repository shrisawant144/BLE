# Central Examples

All examples are in `build/bin/` after building.

## Usage

```bash
cd build/bin
sudo ./ble_scan              # Scan for devices
sudo ./ble_connect <MAC>     # Connect to device
sudo ./ble_read_write <MAC>  # Read/write data
sudo ./ble_notifications <MAC> # Subscribe to notifications
sudo ./heart_rate_monitor <MAC> # Heart rate monitor
sudo ./nordic_uart <MAC>     # Nordic UART client
```

## Examples

1. **ble_scan** - Discover nearby BLE devices
2. **ble_connect** - Connect and explore GATT services
3. **ble_read_write** - Read/write characteristics
4. **ble_notifications** - Subscribe to real-time updates
5. **heart_rate_monitor** - Heart rate service client
6. **nordic_uart** - Serial communication over BLE
