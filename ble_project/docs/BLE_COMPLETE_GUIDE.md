# Complete BLE (Bluetooth Low Energy) Teaching Guide

## Table of Contents
1. [What is BLE?](#what-is-ble)
2. [BLE vs Classic Bluetooth](#ble-vs-classic-bluetooth)
3. [BLE Architecture](#ble-architecture)
4. [Device Roles](#device-roles)
5. [Connection States](#connection-states)
6. [GATT Protocol](#gatt-protocol)
7. [Services and Characteristics](#services-and-characteristics)
8. [UUIDs Explained](#uuids-explained)
9. [Advertising](#advertising)
10. [Connection Process](#connection-process)
11. [Data Transfer](#data-transfer)
12. [Security](#security)
13. [Power Management](#power-management)
14. [Practical Examples](#practical-examples)

---

## What is BLE?

**Bluetooth Low Energy (BLE)** is a wireless communication protocol designed for short-range communication with minimal power consumption. It's part of Bluetooth 4.0+ specification.

### Key Characteristics:
- **Low Power**: Devices can run for months/years on a single battery
- **Short Range**: Typically 10-100 meters
- **Low Data Rate**: Up to 1 Mbps (much slower than WiFi)
- **Quick Connection**: Connects in milliseconds
- **Small Data Packets**: Optimized for small, frequent data exchanges

### Common Use Cases:
- Fitness trackers (heart rate, steps)
- Smart home sensors (temperature, humidity)
- Beacons (location services)
- Medical devices (glucose monitors)
- Wearables (smartwatches)

---

## BLE vs Classic Bluetooth

| Feature | Classic Bluetooth | BLE |
|---------|------------------|-----|
| Power Consumption | High | Very Low |
| Connection Time | ~6 seconds | ~6 milliseconds |
| Data Rate | Up to 24 Mbps | Up to 1 Mbps |
| Range | 10-100m | 10-100m |
| Use Case | Audio, file transfer | Sensors, IoT |
| Always Connected | Yes | No (can sleep) |

---

## BLE Architecture

BLE uses a **layered architecture** similar to the OSI model:

```
┌─────────────────────────┐
│     Application         │ ← Your app code
├─────────────────────────┤
│     GATT (Generic       │ ← Services/Characteristics
│     Attribute Profile)  │
├─────────────────────────┤
│     ATT (Attribute      │ ← Read/Write operations
│     Protocol)           │
├─────────────────────────┤
│     L2CAP (Logical      │ ← Packet routing
│     Link Control)       │
├─────────────────────────┤
│     HCI (Host           │ ← Host-Controller Interface
│     Controller Interface)│
├─────────────────────────┤
│     Link Layer          │ ← Advertising, connections
├─────────────────────────┤
│     Physical Layer      │ ← Radio (2.4 GHz)
└─────────────────────────┘
```

### Layer Explanations:

**Physical Layer**: 2.4 GHz ISM band, 40 channels (3 advertising + 37 data)

**Link Layer**: Handles advertising, scanning, connections, packet timing

**HCI**: Interface between software (host) and hardware (controller)

**L2CAP**: Routes packets, handles fragmentation

**ATT**: Defines how data is stored and accessed (attributes)

**GATT**: Defines services and characteristics structure

**Application**: Your actual program logic

---

## Device Roles

### 1. Central (Client/Master)
**What it does:**
- Scans for advertising devices
- Initiates connections
- Reads/writes data from peripherals
- Can connect to multiple peripherals

**Examples:** Smartphones, laptops, tablets

**Code Pattern:**
```c
// Scan for devices
scan_for_devices();

// Connect to device
connect_to_device("AA:BB:CC:DD:EE:FF");

// Read data
read_characteristic(service_uuid, char_uuid);
```

### 2. Peripheral (Server/Slave)
**What it does:**
- Advertises its presence
- Accepts connections from centrals
- Provides data through services/characteristics
- Usually connects to only one central

**Examples:** Fitness trackers, sensors, beacons

**Code Pattern:**
```c
// Create service
create_service(SERVICE_UUID);

// Add characteristic
add_characteristic(CHAR_UUID, READ|WRITE|NOTIFY);

// Start advertising
start_advertising("Device Name");
```

### 3. Observer
- Only scans, never connects
- Used for beacon detection

### 4. Broadcaster
- Only advertises, never accepts connections
- Used for beacons

---

## Connection States

BLE devices have 5 main states:

### 1. Standby
- Device is idle
- Not advertising or scanning
- Lowest power consumption

### 2. Advertising
- Peripheral broadcasts its presence
- Sends advertising packets every 20ms-10.24s
- Anyone can see these packets

### 3. Scanning
- Central listens for advertising packets
- Can be passive (just listen) or active (request more info)

### 4. Initiating
- Central is attempting to connect to a peripheral
- Short transition state

### 5. Connected
- Two devices have established a connection
- Can exchange data
- Connection has parameters (interval, timeout, etc.)

**State Diagram:**
```
Standby ←→ Advertising (Peripheral)
   ↓           ↓
   ↓      Connected ←→ Initiating ←→ Scanning (Central)
   ↓           ↑                        ↑
   └───────────┴────────────────────────┘
```

---

## GATT Protocol

**GATT (Generic Attribute Profile)** defines how BLE devices exchange data.

### GATT Hierarchy:
```
Device
├── Service 1 (e.g., Heart Rate)
│   ├── Characteristic 1 (Heart Rate Measurement)
│   │   ├── Value (actual heart rate data)
│   │   └── Descriptors (metadata)
│   └── Characteristic 2 (Body Sensor Location)
│       ├── Value (chest, wrist, etc.)
│       └── Descriptors
└── Service 2 (e.g., Battery)
    └── Characteristic 1 (Battery Level)
        ├── Value (percentage)
        └── Descriptors
```

### GATT Roles:
- **GATT Server**: Device that provides data (usually peripheral)
- **GATT Client**: Device that requests data (usually central)

### Key Concepts:

**Attribute**: Basic data element with:
- Handle (unique ID)
- Type (UUID)
- Value (actual data)
- Permissions (read/write/etc.)

---

## Services and Characteristics

### Services
A **service** is a collection of related functionality.

**Structure:**
- Has a UUID (16-bit for standard, 128-bit for custom)
- Contains one or more characteristics
- Can include other services

**Examples:**
- Heart Rate Service (0x180D)
- Battery Service (0x180F)
- Device Information Service (0x180A)

### Characteristics
A **characteristic** is a single data point within a service.

**Properties** (what you can do with it):
- **Read**: Get the current value
- **Write**: Set a new value
- **Write Without Response**: Set value, no confirmation
- **Notify**: Server sends updates automatically
- **Indicate**: Like notify, but with confirmation
- **Broadcast**: Include in advertising packets

**Example - Heart Rate Characteristic:**
```
Service: Heart Rate (0x180D)
├── Characteristic: Heart Rate Measurement (0x2A37)
│   ├── Properties: Read, Notify
│   ├── Value: [0x16, 0x4C] (76 BPM)
│   └── Descriptor: Client Characteristic Configuration
└── Characteristic: Body Sensor Location (0x2A38)
    ├── Properties: Read
    └── Value: [0x01] (Chest)
```

### Descriptors
**Descriptors** provide metadata about characteristics:
- **Client Characteristic Configuration (0x2902)**: Enable/disable notifications
- **Characteristic User Description (0x2901)**: Human-readable description
- **Characteristic Format (0x2904)**: Data format information

---

## UUIDs Explained

**UUID (Universally Unique Identifier)** identifies services and characteristics.

### Types:

**16-bit UUIDs** (Standard):
- Defined by Bluetooth SIG
- Examples: 0x180D (Heart Rate), 0x180F (Battery)
- Actually 128-bit: `0000180D-0000-1000-8000-00805F9B34FB`

**128-bit UUIDs** (Custom):
- For your own services/characteristics
- Example: `12345678-1234-1234-1234-123456789ABC`
- Generate online or with `uuidgen` command

### Common Standard UUIDs:

**Services:**
- `0x1800` - Generic Access
- `0x1801` - Generic Attribute
- `0x180A` - Device Information
- `0x180D` - Heart Rate
- `0x180F` - Battery Service

**Characteristics:**
- `0x2A00` - Device Name
- `0x2A01` - Appearance
- `0x2A19` - Battery Level
- `0x2A37` - Heart Rate Measurement

---

## Advertising

**Advertising** is how peripherals announce their presence.

### Advertising Packets
Sent every 20ms to 10.24 seconds, contains:
- **Device Name** (optional)
- **Service UUIDs** (what services are available)
- **Manufacturer Data** (custom data)
- **TX Power Level** (signal strength)
- **Flags** (device capabilities)

### Advertising Types:
1. **Connectable Undirected**: Anyone can connect
2. **Connectable Directed**: Only specific device can connect
3. **Non-connectable Undirected**: Broadcast only (beacons)
4. **Scannable Undirected**: Can request more info, but not connect

### Scan Response
Additional data sent when central requests more information.

**Example Advertising Packet:**
```
Flags: [0x06] (General Discoverable, BR/EDR Not Supported)
Complete Local Name: "Heart Rate Monitor"
Complete List of 16-bit Service UUIDs: [0x180D]
TX Power Level: [0x00] (0 dBm)
```

---

## Connection Process

### Step-by-Step Connection:

1. **Peripheral Advertising**
   ```
   Peripheral: "I'm here! I'm a heart rate monitor!"
   (Broadcasts every 100ms)
   ```

2. **Central Scanning**
   ```
   Central: "Looking for devices..."
   Central: "Found heart rate monitor at AA:BB:CC:DD:EE:FF"
   ```

3. **Connection Request**
   ```
   Central → Peripheral: "I want to connect"
   ```

4. **Connection Established**
   ```
   Peripheral → Central: "OK, connected!"
   ```

5. **Service Discovery**
   ```
   Central → Peripheral: "What services do you have?"
   Peripheral → Central: "I have Heart Rate Service (0x180D)"
   ```

6. **Characteristic Discovery**
   ```
   Central → Peripheral: "What characteristics in Heart Rate Service?"
   Peripheral → Central: "Heart Rate Measurement (0x2A37) - Read, Notify"
   ```

### Connection Parameters:
- **Connection Interval**: How often devices communicate (7.5ms - 4s)
- **Slave Latency**: How many intervals peripheral can skip
- **Supervision Timeout**: How long before connection is considered lost

---

## Data Transfer

### Reading Data
```c
// Central reads from peripheral
uint8_t data[20];
int len = read_characteristic(service_uuid, char_uuid, data, sizeof(data));
printf("Read %d bytes: ", len);
for(int i = 0; i < len; i++) {
    printf("%02X ", data[i]);
}
```

### Writing Data
```c
// Central writes to peripheral
uint8_t data[] = {0x01, 0x02, 0x03};
write_characteristic(service_uuid, char_uuid, data, sizeof(data));
```

### Notifications
```c
// Peripheral sends automatic updates
void on_timer() {
    uint8_t heart_rate = get_heart_rate();
    notify_characteristic(HR_SERVICE_UUID, HR_MEASUREMENT_UUID, &heart_rate, 1);
}

// Central receives notifications
void on_notification(uint8_t *data, int len) {
    printf("Heart rate: %d BPM\n", data[0]);
}
```

### Data Formats
BLE characteristics can contain:
- **Raw bytes**: `[0x4C]` = 76
- **Strings**: `"Hello World"`
- **Structured data**: Temperature + humidity in one packet
- **Bit fields**: Multiple boolean values in one byte

---

## Security

BLE has multiple security levels:

### 1. No Security
- Data sent in plain text
- Anyone can read/write
- Suitable for non-sensitive data

### 2. Unauthenticated Pairing
- Data is encrypted
- No verification of device identity
- Protection against passive eavesdropping

### 3. Authenticated Pairing
- Data is encrypted
- Devices verify each other's identity
- Protection against man-in-the-middle attacks

### 4. Authenticated Pairing with OOB
- Uses out-of-band method (NFC, QR code)
- Highest security level

### Pairing Process:
1. **Pairing Request**: Central requests to pair
2. **Capability Exchange**: Devices share security capabilities
3. **Key Generation**: Create encryption keys
4. **Key Distribution**: Share keys securely
5. **Bonding**: Store keys for future connections

---

## Power Management

BLE's low power comes from:

### 1. Sleep Modes
- Devices sleep between communications
- Wake up only when needed
- Deep sleep uses microamps

### 2. Connection Intervals
- Longer intervals = less power
- Trade-off between power and responsiveness

### 3. Advertising Intervals
- Longer intervals = less power
- Trade-off between discoverability and power

### 4. Data Efficiency
- Small packets
- Compress data when possible
- Batch multiple readings

### Power Consumption Examples:
- **Advertising**: 10-50 µA average
- **Connected (idle)**: 1-10 µA average
- **Data transfer**: 10-20 mA peak
- **Sleep**: 0.1-1 µA

---

## Practical Examples

### Example 1: Temperature Sensor

**Peripheral Code Concept:**
```c
// Create temperature service
service = create_service("12345678-1234-1234-1234-123456789ABC");

// Add temperature characteristic (read + notify)
temp_char = add_characteristic(service, 
                              "12345678-1234-1234-1234-123456789ABD",
                              READ | NOTIFY);

// Start advertising
start_advertising("Temp Sensor");

// Send temperature every 5 seconds
while(1) {
    float temp = read_temperature();
    uint8_t temp_bytes[4];
    memcpy(temp_bytes, &temp, 4);
    notify_characteristic(temp_char, temp_bytes, 4);
    sleep(5);
}
```

**Central Code Concept:**
```c
// Scan and connect
scan_for_devices();
connect_to_device("AA:BB:CC:DD:EE:FF");

// Discover services
discover_services();

// Enable notifications
enable_notifications("12345678-1234-1234-1234-123456789ABD");

// Handle notifications
void on_notification(uint8_t *data, int len) {
    float temp;
    memcpy(&temp, data, 4);
    printf("Temperature: %.1f°C\n", temp);
}
```

### Example 2: LED Controller

**Peripheral (LED Controller):**
- Service: LED Control
- Characteristic: LED State (Write)
  - 0x00 = OFF
  - 0x01 = ON
  - 0x02 = BLINK

**Central (Phone App):**
- Scan for LED controller
- Connect and discover services
- Write commands to LED characteristic

### Example 3: Fitness Tracker

**Services:**
1. **Heart Rate Service (0x180D)**
   - Heart Rate Measurement (Notify)
   - Body Sensor Location (Read)

2. **Battery Service (0x180F)**
   - Battery Level (Read + Notify)

3. **Device Information (0x180A)**
   - Manufacturer Name (Read)
   - Model Number (Read)
   - Firmware Revision (Read)

---

## Debugging Tips

### Common Issues:

**1. Can't find device**
- Check if advertising
- Verify device is in range
- Check advertising interval

**2. Connection fails**
- Device might be connected to another central
- Check connection parameters
- Verify MAC address

**3. Can't read/write characteristic**
- Check characteristic properties
- Verify permissions
- Check if pairing is required

**4. Notifications not working**
- Enable notifications via descriptor (0x2902)
- Check characteristic supports notify
- Verify connection is stable

### Debugging Tools:
- **nRF Connect** (mobile app)
- **Wireshark** (packet capture)
- **hcitool/gatttool** (Linux command line)
- **Bluetooth HCI snoop log** (Android)

---

## Summary

BLE is a powerful protocol for low-power, short-range communication:

1. **Devices** have roles (Central/Peripheral)
2. **Services** group related functionality
3. **Characteristics** are individual data points
4. **Advertising** announces device presence
5. **GATT** defines data structure
6. **Security** protects sensitive data
7. **Power management** enables long battery life

The key to BLE development is understanding the GATT hierarchy and how data flows between central and peripheral devices through services and characteristics.