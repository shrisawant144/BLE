/**
 * @file simple_peripheral.cpp
 * @brief Simple BLE Peripheral (GATT Server) using BlueZ D-Bus API
 * 
 * Creates a GATT server with a custom service containing:
 * - A readable/writable characteristic
 * 
 * Build:
 *   g++ -o simple_peripheral simple_peripheral.cpp \
 *       $(pkg-config --cflags --libs gio-2.0) -Wall
 * 
 * Run:
 *   sudo ./simple_peripheral
 */

#include <gio/gio.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>

// Custom Service UUID
#define SERVICE_UUID        "12345678-1234-5678-1234-56789abcdef0"
#define CHARACTERISTIC_UUID "12345678-1234-5678-1234-56789abcdef1"

// D-Bus paths
#define APP_PATH            "/org/bluez/example"
#define SERVICE_PATH        "/org/bluez/example/service0"
#define CHAR_PATH           "/org/bluez/example/service0/char0"
#define ADVERT_PATH         "/org/bluez/example/advertisement0"

// Global state
static GMainLoop *main_loop = NULL;
static GDBusConnection *connection = NULL;
static guint app_registration_id = 0;
static guint service_registration_id = 0;
static guint char_registration_id = 0;
static guint advert_registration_id = 0;

// Characteristic value storage
static char char_value[256] = "Hello BLE!";
static size_t char_value_len = 10;

// Signal handler for clean shutdown
static void signal_handler(int sig) {
    printf("\n🛑 Shutting down...\n");
    if (main_loop) {
        g_main_loop_quit(main_loop);
    }
}

// =============================================================================
// Characteristic Implementation
// =============================================================================

static void handle_char_method_call(
    GDBusConnection *conn,
    const gchar *sender,
    const gchar *object_path,
    const gchar *interface_name,
    const gchar *method_name,
    GVariant *parameters,
    GDBusMethodInvocation *invocation,
    gpointer user_data)
{
    if (g_strcmp0(method_name, "ReadValue") == 0) {
        printf("📖 Read request received\n");
        printf("   Value: \"%s\"\n", char_value);
        
        GVariantBuilder *builder = g_variant_builder_new(G_VARIANT_TYPE("ay"));
        for (size_t i = 0; i < char_value_len; i++) {
            g_variant_builder_add(builder, "y", (guchar)char_value[i]);
        }
        
        GVariant *result = g_variant_new("(ay)", builder);
        g_variant_builder_unref(builder);
        g_dbus_method_invocation_return_value(invocation, result);
    }
    else if (g_strcmp0(method_name, "WriteValue") == 0) {
        GVariant *value_variant;
        GVariant *options;
        g_variant_get(parameters, "(@aya{sv})", &value_variant, &options);
        
        gsize n_elements;
        const guchar *value = (const guchar *)g_variant_get_fixed_array(
            value_variant, &n_elements, sizeof(guchar));
        
        // Store the new value
        char_value_len = n_elements < sizeof(char_value) - 1 ? n_elements : sizeof(char_value) - 1;
        memcpy(char_value, value, char_value_len);
        char_value[char_value_len] = '\0';
        
        printf("✏️  Write request received\n");
        printf("   New value: \"%s\"\n", char_value);
        
        g_variant_unref(value_variant);
        g_variant_unref(options);
        g_dbus_method_invocation_return_value(invocation, NULL);
    }
    else {
        g_dbus_method_invocation_return_error(invocation,
            G_DBUS_ERROR, G_DBUS_ERROR_NOT_SUPPORTED,
            "Method %s not supported", method_name);
    }
}

static GVariant* handle_char_get_property(
    GDBusConnection *conn,
    const gchar *sender,
    const gchar *object_path,
    const gchar *interface_name,
    const gchar *property_name,
    GError **error,
    gpointer user_data)
{
    if (g_strcmp0(property_name, "UUID") == 0) {
        return g_variant_new_string(CHARACTERISTIC_UUID);
    }
    else if (g_strcmp0(property_name, "Service") == 0) {
        return g_variant_new_object_path(SERVICE_PATH);
    }
    else if (g_strcmp0(property_name, "Flags") == 0) {
        const gchar *flags[] = {"read", "write", NULL};
        return g_variant_new_strv(flags, -1);
    }
    
    return NULL;
}

static const GDBusInterfaceVTable char_vtable = {
    handle_char_method_call,
    handle_char_get_property,
    NULL
};

// =============================================================================
// Service Implementation
// =============================================================================

static GVariant* handle_service_get_property(
    GDBusConnection *conn,
    const gchar *sender,
    const gchar *object_path,
    const gchar *interface_name,
    const gchar *property_name,
    GError **error,
    gpointer user_data)
{
    if (g_strcmp0(property_name, "UUID") == 0) {
        return g_variant_new_string(SERVICE_UUID);
    }
    else if (g_strcmp0(property_name, "Primary") == 0) {
        return g_variant_new_boolean(TRUE);
    }
    else if (g_strcmp0(property_name, "Characteristics") == 0) {
        GVariantBuilder builder;
        g_variant_builder_init(&builder, G_VARIANT_TYPE("ao"));
        g_variant_builder_add(&builder, "o", CHAR_PATH);
        return g_variant_builder_end(&builder);
    }
    
    return NULL;
}

static const GDBusInterfaceVTable service_vtable = {
    NULL,
    handle_service_get_property,
    NULL
};

// =============================================================================
// Application (Object Manager)
// =============================================================================

static void handle_app_method_call(
    GDBusConnection *conn,
    const gchar *sender,
    const gchar *object_path,
    const gchar *interface_name,
    const gchar *method_name,
    GVariant *parameters,
    GDBusMethodInvocation *invocation,
    gpointer user_data)
{
    if (g_strcmp0(method_name, "GetManagedObjects") == 0) {
        GVariantBuilder *builder = g_variant_builder_new(G_VARIANT_TYPE("a{oa{sa{sv}}}"));
        
        // Add Service
        GVariantBuilder *service_props = g_variant_builder_new(G_VARIANT_TYPE("a{sv}"));
        g_variant_builder_add(service_props, "{sv}", "UUID", g_variant_new_string(SERVICE_UUID));
        g_variant_builder_add(service_props, "{sv}", "Primary", g_variant_new_boolean(TRUE));
        
        GVariantBuilder *service_ifaces = g_variant_builder_new(G_VARIANT_TYPE("a{sa{sv}}"));
        g_variant_builder_add(service_ifaces, "{sa{sv}}", "org.bluez.GattService1", service_props);
        g_variant_builder_add(builder, "{oa{sa{sv}}}", SERVICE_PATH, service_ifaces);
        
        // Add Characteristic
        GVariantBuilder *char_props = g_variant_builder_new(G_VARIANT_TYPE("a{sv}"));
        g_variant_builder_add(char_props, "{sv}", "UUID", g_variant_new_string(CHARACTERISTIC_UUID));
        g_variant_builder_add(char_props, "{sv}", "Service", g_variant_new_object_path(SERVICE_PATH));
        const gchar *flags[] = {"read", "write", NULL};
        g_variant_builder_add(char_props, "{sv}", "Flags", g_variant_new_strv(flags, -1));
        
        GVariantBuilder *char_ifaces = g_variant_builder_new(G_VARIANT_TYPE("a{sa{sv}}"));
        g_variant_builder_add(char_ifaces, "{sa{sv}}", "org.bluez.GattCharacteristic1", char_props);
        g_variant_builder_add(builder, "{oa{sa{sv}}}", CHAR_PATH, char_ifaces);
        
        GVariant *result = g_variant_new("(a{oa{sa{sv}}})", builder);
        g_dbus_method_invocation_return_value(invocation, result);
    }
}

static const GDBusInterfaceVTable app_vtable = {
    handle_app_method_call,
    NULL,
    NULL
};

// =============================================================================
// Advertisement
// =============================================================================

static void handle_advert_method_call(
    GDBusConnection *conn,
    const gchar *sender,
    const gchar *object_path,
    const gchar *interface_name,
    const gchar *method_name,
    GVariant *parameters,
    GDBusMethodInvocation *invocation,
    gpointer user_data)
{
    if (g_strcmp0(method_name, "Release") == 0) {
        printf("📢 Advertisement released\n");
        g_dbus_method_invocation_return_value(invocation, NULL);
    }
}

static GVariant* handle_advert_get_property(
    GDBusConnection *conn,
    const gchar *sender,
    const gchar *object_path,
    const gchar *interface_name,
    const gchar *property_name,
    GError **error,
    gpointer user_data)
{
    if (g_strcmp0(property_name, "Type") == 0) {
        return g_variant_new_string("peripheral");
    }
    else if (g_strcmp0(property_name, "ServiceUUIDs") == 0) {
        const gchar *uuids[] = {SERVICE_UUID, NULL};
        return g_variant_new_strv(uuids, -1);
    }
    else if (g_strcmp0(property_name, "LocalName") == 0) {
        return g_variant_new_string("Simple-Peripheral");
    }
    else if (g_strcmp0(property_name, "Includes") == 0) {
        const gchar *includes[] = {"tx-power", NULL};
        return g_variant_new_strv(includes, -1);
    }
    
    return NULL;
}

static const GDBusInterfaceVTable advert_vtable = {
    handle_advert_method_call,
    handle_advert_get_property,
    NULL
};

// =============================================================================
// D-Bus Interface XML
// =============================================================================

static const gchar app_introspection_xml[] =
    "<node>"
    "  <interface name='org.freedesktop.DBus.ObjectManager'>"
    "    <method name='GetManagedObjects'>"
    "      <arg type='a{oa{sa{sv}}}' direction='out'/>"
    "    </method>"
    "  </interface>"
    "</node>";

static const gchar service_introspection_xml[] =
    "<node>"
    "  <interface name='org.bluez.GattService1'>"
    "    <property name='UUID' type='s' access='read'/>"
    "    <property name='Primary' type='b' access='read'/>"
    "    <property name='Characteristics' type='ao' access='read'/>"
    "  </interface>"
    "</node>";

static const gchar char_introspection_xml[] =
    "<node>"
    "  <interface name='org.bluez.GattCharacteristic1'>"
    "    <method name='ReadValue'>"
    "      <arg type='a{sv}' direction='in'/>"
    "      <arg type='ay' direction='out'/>"
    "    </method>"
    "    <method name='WriteValue'>"
    "      <arg type='ay' direction='in'/>"
    "      <arg type='a{sv}' direction='in'/>"
    "    </method>"
    "    <property name='UUID' type='s' access='read'/>"
    "    <property name='Service' type='o' access='read'/>"
    "    <property name='Flags' type='as' access='read'/>"
    "  </interface>"
    "</node>";

static const gchar advert_introspection_xml[] =
    "<node>"
    "  <interface name='org.bluez.LEAdvertisement1'>"
    "    <method name='Release'/>"
    "    <property name='Type' type='s' access='read'/>"
    "    <property name='ServiceUUIDs' type='as' access='read'/>"
    "    <property name='LocalName' type='s' access='read'/>"
    "    <property name='Includes' type='as' access='read'/>"
    "  </interface>"
    "</node>";

// =============================================================================
// Registration Callbacks
// =============================================================================

static void on_register_application_reply(GObject *source, GAsyncResult *res, gpointer user_data) {
    GError *error = NULL;
    GVariant *result = g_dbus_connection_call_finish(connection, res, &error);
    
    if (error) {
        printf("❌ Failed to register application: %s\n", error->message);
        g_error_free(error);
        g_main_loop_quit(main_loop);
        return;
    }
    
    printf("✅ GATT application registered\n");
    if (result) g_variant_unref(result);
}

static void on_register_advertisement_reply(GObject *source, GAsyncResult *res, gpointer user_data) {
    GError *error = NULL;
    GVariant *result = g_dbus_connection_call_finish(connection, res, &error);
    
    if (error) {
        printf("❌ Failed to register advertisement: %s\n", error->message);
        g_error_free(error);
        return;
    }
    
    printf("✅ Advertisement registered\n");
    printf("\n📡 Peripheral is now advertising as 'Simple-Peripheral'\n");
    printf("   Service UUID: %s\n", SERVICE_UUID);
    printf("\n   Waiting for connections... (Ctrl+C to stop)\n\n");
    
    if (result) g_variant_unref(result);
}

// =============================================================================
// Main
// =============================================================================

int main(int argc, char *argv[]) {
    GError *error = NULL;
    
    printf("╔════════════════════════════════════════════════════════════╗\n");
    printf("║           Simple BLE Peripheral (C++ Version)              ║\n");
    printf("╚════════════════════════════════════════════════════════════╝\n\n");
    
    // Setup signal handler
    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);
    
    // Connect to system bus
    connection = g_bus_get_sync(G_BUS_TYPE_SYSTEM, NULL, &error);
    if (!connection) {
        printf("❌ Failed to connect to system bus: %s\n", error->message);
        g_error_free(error);
        return 1;
    }
    printf("✅ Connected to D-Bus\n");
    
    // Parse introspection data
    GDBusNodeInfo *app_node = g_dbus_node_info_new_for_xml(app_introspection_xml, &error);
    GDBusNodeInfo *service_node = g_dbus_node_info_new_for_xml(service_introspection_xml, &error);
    GDBusNodeInfo *char_node = g_dbus_node_info_new_for_xml(char_introspection_xml, &error);
    GDBusNodeInfo *advert_node = g_dbus_node_info_new_for_xml(advert_introspection_xml, &error);
    
    if (!app_node || !service_node || !char_node || !advert_node) {
        printf("❌ Failed to parse introspection XML\n");
        return 1;
    }
    
    // Register Application (ObjectManager)
    app_registration_id = g_dbus_connection_register_object(
        connection, APP_PATH,
        app_node->interfaces[0],
        &app_vtable, NULL, NULL, &error);
    
    // Register Service
    service_registration_id = g_dbus_connection_register_object(
        connection, SERVICE_PATH,
        service_node->interfaces[0],
        &service_vtable, NULL, NULL, &error);
    
    // Register Characteristic
    char_registration_id = g_dbus_connection_register_object(
        connection, CHAR_PATH,
        char_node->interfaces[0],
        &char_vtable, NULL, NULL, &error);
    
    // Register Advertisement
    advert_registration_id = g_dbus_connection_register_object(
        connection, ADVERT_PATH,
        advert_node->interfaces[0],
        &advert_vtable, NULL, NULL, &error);
    
    if (error) {
        printf("❌ Failed to register objects: %s\n", error->message);
        g_error_free(error);
        return 1;
    }
    
    printf("✅ D-Bus objects registered\n");
    
    // Register GATT Application with BlueZ
    g_dbus_connection_call(
        connection,
        "org.bluez",
        "/org/bluez/hci0",
        "org.bluez.GattManager1",
        "RegisterApplication",
        g_variant_new("(oa{sv})", APP_PATH, NULL),
        NULL,
        G_DBUS_CALL_FLAGS_NONE,
        -1,
        NULL,
        on_register_application_reply,
        NULL);
    
    // Register Advertisement with BlueZ
    g_dbus_connection_call(
        connection,
        "org.bluez",
        "/org/bluez/hci0",
        "org.bluez.LEAdvertisingManager1",
        "RegisterAdvertisement",
        g_variant_new("(oa{sv})", ADVERT_PATH, NULL),
        NULL,
        G_DBUS_CALL_FLAGS_NONE,
        -1,
        NULL,
        on_register_advertisement_reply,
        NULL);
    
    // Run main loop
    main_loop = g_main_loop_new(NULL, FALSE);
    g_main_loop_run(main_loop);
    
    // Cleanup
    printf("\n🧹 Cleaning up...\n");
    
    // Unregister advertisement
    g_dbus_connection_call_sync(
        connection,
        "org.bluez",
        "/org/bluez/hci0",
        "org.bluez.LEAdvertisingManager1",
        "UnregisterAdvertisement",
        g_variant_new("(o)", ADVERT_PATH),
        NULL,
        G_DBUS_CALL_FLAGS_NONE,
        -1, NULL, NULL);
    
    // Unregister application
    g_dbus_connection_call_sync(
        connection,
        "org.bluez",
        "/org/bluez/hci0",
        "org.bluez.GattManager1",
        "UnregisterApplication",
        g_variant_new("(o)", APP_PATH),
        NULL,
        G_DBUS_CALL_FLAGS_NONE,
        -1, NULL, NULL);
    
    g_dbus_connection_unregister_object(connection, app_registration_id);
    g_dbus_connection_unregister_object(connection, service_registration_id);
    g_dbus_connection_unregister_object(connection, char_registration_id);
    g_dbus_connection_unregister_object(connection, advert_registration_id);
    
    g_dbus_node_info_unref(app_node);
    g_dbus_node_info_unref(service_node);
    g_dbus_node_info_unref(char_node);
    g_dbus_node_info_unref(advert_node);
    
    g_main_loop_unref(main_loop);
    g_object_unref(connection);
    
    printf("✅ Done!\n");
    return 0;
}
