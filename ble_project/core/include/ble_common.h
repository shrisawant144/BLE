#ifndef BLE_COMMON_H
#define BLE_COMMON_H

#include <stdint.h>
#include <stdbool.h>

#define BLE_UUID_SIZE 37
#define BLE_ADDR_SIZE 18

typedef struct {
    char address[BLE_ADDR_SIZE];
    char name[256];
    int16_t rssi;
} ble_device_t;

const char* ble_uuid_to_name(const char* uuid);
bool ble_is_valid_address(const char* address);
void ble_print_device(const ble_device_t* device);

#endif
