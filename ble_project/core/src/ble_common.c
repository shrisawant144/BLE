#include "ble_common.h"
#include <stdio.h>
#include <string.h>
#include <ctype.h>

const char* ble_uuid_to_name(const char* uuid) {
    if (strcmp(uuid, "0000180d-0000-1000-8000-00805f9b34fb") == 0) return "Heart Rate";
    if (strcmp(uuid, "0000180f-0000-1000-8000-00805f9b34fb") == 0) return "Battery";
    if (strcmp(uuid, "0000180a-0000-1000-8000-00805f9b34fb") == 0) return "Device Info";
    if (strcmp(uuid, "6e400001-b5a3-f393-e0a9-e50e24dcca9e") == 0) return "Nordic UART";
    return "Unknown";
}

bool ble_is_valid_address(const char* address) {
    if (!address || strlen(address) != 17) return false;
    for (int i = 0; i < 17; i++) {
        if (i % 3 == 2) {
            if (address[i] != ':') return false;
        } else {
            if (!isxdigit(address[i])) return false;
        }
    }
    return true;
}

void ble_print_device(const ble_device_t* device) {
    printf("Device: %s\n", device->name[0] ? device->name : "Unknown");
    printf("  Address: %s\n", device->address);
    printf("  RSSI: %d dBm\n", device->rssi);
}
