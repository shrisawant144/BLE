# BLE Documentation

## Central Examples (Client)

### 1. ble_scan
**Purpose:** Discover nearby BLE devices  
**Usage:** `sudo ./ble_scan`  
**What it does:** Scans for 10 seconds and lists all BLE devices with their MAC addresses and names

### 2. ble_connect
**Purpose:** Connect to a device and explore its structure  
**Usage:** `sudo ./ble_connect AA:BB:CC:DD:EE:FF`  
**What it does:** Connects to device and lists all services and characteristics with their properties (Read/Write/Notify)

### 3. ble_rw
**Purpose:** Read or write characteristic values  
**Usage:**  
- Read: `sudo ./ble_rw AA:BB:CC:DD:EE:FF <UUID>`
- Write: `sudo ./ble_rw AA:BB:CC:DD:EE:FF <UUID> "value"`

**What it does:** Reads data from or writes data to a specific characteristic

### 4. ble_notify
**Purpose:** Subscribe to real-time notifications  
**Usage:** `sudo ./ble_notify AA:BB:CC:DD:EE:FF <UUID>`  
**What it does:** Subscribes to a characteristic and prints all notifications received (Ctrl+C to stop)

## Peripheral Examples (Server)

### 1. simple_peripheral
**Purpose:** Basic GATT server with read/write  
**Usage:** `sudo ./simple_peripheral`  
**What it does:** Creates a BLE device named "Simple-Peripheral" with one characteristic that can be read and written

### 2. ble_peripheral_notify
**Purpose:** GATT server that sends notifications  
**Usage:** `sudo ./ble_peripheral_notify`  
**What it does:** Creates a BLE device named "BLE-Notify" with a counter that increments every 2 seconds

## Quick Start

```bash
# Build all examples
make build

# Terminal 1: Start peripheral
cd build/bin
sudo ./simple_peripheral

# Terminal 2: Scan for it
sudo ./ble_scan

# Terminal 3: Connect to it
sudo ./ble_connect AA:BB:CC:DD:EE:FF
```

## BLE Concepts

**Central/Client** - Scans, connects, reads/writes (your phone, laptop)  
**Peripheral/Server** - Advertises, accepts connections, provides data (sensors, wearables)

**Service** - Group of related characteristics (UUID like 0x180F for Battery)  
**Characteristic** - Individual data point with properties (Read, Write, Notify)

**Common UUIDs:**
- `0x180D` - Heart Rate Service
- `0x180F` - Battery Service
- `0x180A` - Device Information

## Troubleshooting

**Adapter not found:**
```bash
sudo systemctl start bluetooth
sudo hciconfig hci0 up
```

**Permission denied:**
```bash
sudo ./your_program
```

**Connection fails:**
- Device must be advertising
- Check MAC address is correct
- Ensure device is in range
