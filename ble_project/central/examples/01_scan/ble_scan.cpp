// BLE Scanner - Discover nearby Bluetooth devices
// Usage: sudo ./ble_scan

#include <iostream>
#include <iomanip>
#include <gattlib.h>

#define SCAN_DURATION 10

void on_device_found(gattlib_adapter_t* adapter, const char* addr, 
                     const char* name, void* user_data) {
    (void)adapter; (void)user_data;
    
    static int count = 0;
    std::cout << "[" << ++count << "] " << addr;
    if (name) std::cout << " - " << name;
    std::cout << std::endl;
}

void* scan_task(void* arg) {
    gattlib_adapter_t* adapter = (gattlib_adapter_t*)arg;
    
    std::cout << "Scanning for " << SCAN_DURATION << " seconds...\n" << std::endl;
    
    int ret = gattlib_adapter_scan_enable(adapter, on_device_found, 
                                          SCAN_DURATION, nullptr);
    if (ret != GATTLIB_SUCCESS) {
        std::cerr << "Scan failed: " << ret << std::endl;
    }
    
    std::cout << "\nScan complete!" << std::endl;
    gattlib_adapter_close(adapter);
    return nullptr;
}

int main() {
    gattlib_adapter_t* adapter = nullptr;
    
    if (gattlib_adapter_open(nullptr, &adapter) != GATTLIB_SUCCESS) {
        std::cerr << "Failed to open adapter. Try: sudo systemctl start bluetooth" 
                  << std::endl;
        return 1;
    }
    
    gattlib_mainloop(scan_task, adapter);
    return 0;
}
