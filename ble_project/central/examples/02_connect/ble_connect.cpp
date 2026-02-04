// BLE Connect - Connect to device and discover services
// Usage: sudo ./ble_connect AA:BB:CC:DD:EE:FF

#include <iostream>
#include <iomanip>
#include <pthread.h>
#include <unistd.h>
#include <gattlib.h>

static pthread_mutex_t g_mutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t g_cond = PTHREAD_COND_INITIALIZER;
static gattlib_connection_t* g_connection = nullptr;

void on_connect(gattlib_adapter_t* adapter, const char* dst, 
                gattlib_connection_t* connection, int error, void* user_data) {
    (void)adapter; (void)dst; (void)user_data;
    
    pthread_mutex_lock(&g_mutex);
    if (error == GATTLIB_SUCCESS) {
        g_connection = connection;
        std::cout << "Connected!" << std::endl;
    } else {
        std::cerr << "Connection failed: " << error << std::endl;
    }
    pthread_cond_signal(&g_cond);
    pthread_mutex_unlock(&g_mutex);
}

void discover_services(gattlib_connection_t* conn) {
    gattlib_primary_service_t* services;
    int count;
    
    if (gattlib_discover_primary(conn, &services, &count) != GATTLIB_SUCCESS) {
        std::cerr << "Service discovery failed" << std::endl;
        return;
    }
    
    std::cout << "\nServices (" << count << "):" << std::endl;
    for (int i = 0; i < count; i++) {
        char uuid[37];
        gattlib_uuid_to_string(&services[i].uuid, uuid, sizeof(uuid));
        std::cout << "  " << uuid << std::endl;
    }
    free(services);
}

void discover_characteristics(gattlib_connection_t* conn) {
    gattlib_characteristic_t* chars;
    int count;
    
    if (gattlib_discover_char(conn, &chars, &count) != GATTLIB_SUCCESS) {
        std::cerr << "Characteristic discovery failed" << std::endl;
        return;
    }
    
    std::cout << "\nCharacteristics (" << count << "):" << std::endl;
    for (int i = 0; i < count; i++) {
        char uuid[37];
        gattlib_uuid_to_string(&chars[i].uuid, uuid, sizeof(uuid));
        std::cout << "  " << uuid << " [";
        if (chars[i].properties & GATTLIB_CHARACTERISTIC_READ) std::cout << "R";
        if (chars[i].properties & GATTLIB_CHARACTERISTIC_WRITE) std::cout << "W";
        if (chars[i].properties & GATTLIB_CHARACTERISTIC_NOTIFY) std::cout << "N";
        std::cout << "]" << std::endl;
    }
    free(chars);
}

void* connect_task(void* arg) {
    const char* mac = (const char*)arg;
    gattlib_adapter_t* adapter = nullptr;
    
    if (gattlib_adapter_open(nullptr, &adapter) != GATTLIB_SUCCESS) {
        std::cerr << "Failed to open adapter" << std::endl;
        return nullptr;
    }
    
    std::cout << "Connecting to " << mac << "..." << std::endl;
    
    if (gattlib_connect(adapter, mac, GATTLIB_CONNECTION_OPTIONS_NONE, 
                        on_connect, nullptr) != GATTLIB_SUCCESS) {
        std::cerr << "Connection initiation failed" << std::endl;
        gattlib_adapter_close(adapter);
        return nullptr;
    }
    
    pthread_mutex_lock(&g_mutex);
    pthread_cond_wait(&g_cond, &g_mutex);
    pthread_mutex_unlock(&g_mutex);
    
    if (g_connection) {
        discover_services(g_connection);
        discover_characteristics(g_connection);
        
        sleep(2);
        gattlib_disconnect(g_connection, false);
    }
    
    gattlib_adapter_close(adapter);
    return nullptr;
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <MAC_ADDRESS>" << std::endl;
        return 1;
    }
    
    gattlib_mainloop(connect_task, argv[1]);
    return 0;
}
