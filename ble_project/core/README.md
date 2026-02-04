# Core Library

Shared utilities and common functions for BLE operations.

## Contents

- `ble_common.h` - Common data structures and function declarations
- `ble_common.c` - UUID mapping, address validation, device printing

## Usage

The core library is automatically linked when building central or peripheral examples.

```c
#include "ble_common.h"

ble_device_t device;
strcpy(device.address, "AA:BB:CC:DD:EE:FF");
strcpy(device.name, "My Device");
device.rssi = -65;

ble_print_device(&device);
```

## Functions

### ble_uuid_to_name
Converts standard BLE UUIDs to human-readable names.

### ble_is_valid_address
Validates BLE MAC address format.

### ble_print_device
Pretty-prints device information.
