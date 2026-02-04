// BLE Peripheral with Notifications
// Usage: sudo ./ble_peripheral_notify

#include <gio/gio.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>

#define SERVICE_UUID "12345678-1234-5678-1234-56789abcdef0"
#define CHAR_UUID    "12345678-1234-5678-1234-56789abcdef1"
#define APP_PATH     "/org/bluez/example"
#define SERVICE_PATH "/org/bluez/example/service0"
#define CHAR_PATH    "/org/bluez/example/service0/char0"
#define ADVERT_PATH  "/org/bluez/example/advertisement0"

static GMainLoop *main_loop = NULL;
static GDBusConnection *connection = NULL;
static int counter = 0;

static void signal_handler(int sig) {
    (void)sig;
    if (main_loop) g_main_loop_quit(main_loop);
}

static void handle_char_method_call(GDBusConnection *conn, const gchar *sender,
    const gchar *object_path, const gchar *interface_name, const gchar *method_name,
    GVariant *parameters, GDBusMethodInvocation *invocation, gpointer user_data) {
    
    if (g_strcmp0(method_name, "ReadValue") == 0) {
        char value[32];
        snprintf(value, sizeof(value), "Count: %d", counter);
        
        GVariantBuilder *builder = g_variant_builder_new(G_VARIANT_TYPE("ay"));
        for (size_t i = 0; i < strlen(value); i++) {
            g_variant_builder_add(builder, "y", (guchar)value[i]);
        }
        
        g_dbus_method_invocation_return_value(invocation, g_variant_new("(ay)", builder));
        g_variant_builder_unref(builder);
        printf("Read: %s\n", value);
    }
    else if (g_strcmp0(method_name, "StartNotify") == 0) {
        printf("Notifications enabled\n");
        g_dbus_method_invocation_return_value(invocation, NULL);
    }
    else if (g_strcmp0(method_name, "StopNotify") == 0) {
        printf("Notifications disabled\n");
        g_dbus_method_invocation_return_value(invocation, NULL);
    }
}

static GVariant* handle_char_get_property(GDBusConnection *conn, const gchar *sender,
    const gchar *object_path, const gchar *interface_name, const gchar *property_name,
    GError **error, gpointer user_data) {
    
    if (g_strcmp0(property_name, "UUID") == 0) {
        return g_variant_new_string(CHAR_UUID);
    }
    else if (g_strcmp0(property_name, "Service") == 0) {
        return g_variant_new_object_path(SERVICE_PATH);
    }
    else if (g_strcmp0(property_name, "Flags") == 0) {
        const gchar *flags[] = {"read", "notify", NULL};
        return g_variant_new_strv(flags, -1);
    }
    return NULL;
}

static const GDBusInterfaceVTable char_vtable = {
    handle_char_method_call, handle_char_get_property, NULL
};

static GVariant* handle_service_get_property(GDBusConnection *conn, const gchar *sender,
    const gchar *object_path, const gchar *interface_name, const gchar *property_name,
    GError **error, gpointer user_data) {
    
    if (g_strcmp0(property_name, "UUID") == 0) {
        return g_variant_new_string(SERVICE_UUID);
    }
    else if (g_strcmp0(property_name, "Primary") == 0) {
        return g_variant_new_boolean(TRUE);
    }
    return NULL;
}

static const GDBusInterfaceVTable service_vtable = {
    NULL, handle_service_get_property, NULL
};

static void handle_app_method_call(GDBusConnection *conn, const gchar *sender,
    const gchar *object_path, const gchar *interface_name, const gchar *method_name,
    GVariant *parameters, GDBusMethodInvocation *invocation, gpointer user_data) {
    
    if (g_strcmp0(method_name, "GetManagedObjects") == 0) {
        GVariantBuilder *builder = g_variant_builder_new(G_VARIANT_TYPE("a{oa{sa{sv}}}"));
        
        GVariantBuilder *service_props = g_variant_builder_new(G_VARIANT_TYPE("a{sv}"));
        g_variant_builder_add(service_props, "{sv}", "UUID", g_variant_new_string(SERVICE_UUID));
        g_variant_builder_add(service_props, "{sv}", "Primary", g_variant_new_boolean(TRUE));
        
        GVariantBuilder *service_ifaces = g_variant_builder_new(G_VARIANT_TYPE("a{sa{sv}}"));
        g_variant_builder_add(service_ifaces, "{sa{sv}}", "org.bluez.GattService1", service_props);
        g_variant_builder_add(builder, "{oa{sa{sv}}}", SERVICE_PATH, service_ifaces);
        
        GVariantBuilder *char_props = g_variant_builder_new(G_VARIANT_TYPE("a{sv}"));
        g_variant_builder_add(char_props, "{sv}", "UUID", g_variant_new_string(CHAR_UUID));
        g_variant_builder_add(char_props, "{sv}", "Service", g_variant_new_object_path(SERVICE_PATH));
        const gchar *flags[] = {"read", "notify", NULL};
        g_variant_builder_add(char_props, "{sv}", "Flags", g_variant_new_strv(flags, -1));
        
        GVariantBuilder *char_ifaces = g_variant_builder_new(G_VARIANT_TYPE("a{sa{sv}}"));
        g_variant_builder_add(char_ifaces, "{sa{sv}}", "org.bluez.GattCharacteristic1", char_props);
        g_variant_builder_add(builder, "{oa{sa{sv}}}", CHAR_PATH, char_ifaces);
        
        g_dbus_method_invocation_return_value(invocation, g_variant_new("(a{oa{sa{sv}}})", builder));
    }
}

static const GDBusInterfaceVTable app_vtable = {
    handle_app_method_call, NULL, NULL
};

static GVariant* handle_advert_get_property(GDBusConnection *conn, const gchar *sender,
    const gchar *object_path, const gchar *interface_name, const gchar *property_name,
    GError **error, gpointer user_data) {
    
    if (g_strcmp0(property_name, "Type") == 0) {
        return g_variant_new_string("peripheral");
    }
    else if (g_strcmp0(property_name, "ServiceUUIDs") == 0) {
        const gchar *uuids[] = {SERVICE_UUID, NULL};
        return g_variant_new_strv(uuids, -1);
    }
    else if (g_strcmp0(property_name, "LocalName") == 0) {
        return g_variant_new_string("BLE-Notify");
    }
    return NULL;
}

static const GDBusInterfaceVTable advert_vtable = {
    NULL, handle_advert_get_property, NULL
};

static gboolean update_counter(gpointer user_data) {
    counter++;
    printf("Counter: %d\n", counter);
    return TRUE;
}

int main() {
    GError *error = NULL;
    
    printf("BLE Peripheral with Notifications\n\n");
    
    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);
    
    connection = g_bus_get_sync(G_BUS_TYPE_SYSTEM, NULL, &error);
    if (!connection) {
        fprintf(stderr, "Failed to connect to D-Bus\n");
        return 1;
    }
    
    const gchar *app_xml = "<node><interface name='org.freedesktop.DBus.ObjectManager'>"
        "<method name='GetManagedObjects'><arg type='a{oa{sa{sv}}}' direction='out'/></method>"
        "</interface></node>";
    
    const gchar *service_xml = "<node><interface name='org.bluez.GattService1'>"
        "<property name='UUID' type='s' access='read'/>"
        "<property name='Primary' type='b' access='read'/></interface></node>";
    
    const gchar *char_xml = "<node><interface name='org.bluez.GattCharacteristic1'>"
        "<method name='ReadValue'><arg type='a{sv}' direction='in'/><arg type='ay' direction='out'/></method>"
        "<method name='StartNotify'/><method name='StopNotify'/>"
        "<property name='UUID' type='s' access='read'/>"
        "<property name='Service' type='o' access='read'/>"
        "<property name='Flags' type='as' access='read'/></interface></node>";
    
    const gchar *advert_xml = "<node><interface name='org.bluez.LEAdvertisement1'>"
        "<property name='Type' type='s' access='read'/>"
        "<property name='ServiceUUIDs' type='as' access='read'/>"
        "<property name='LocalName' type='s' access='read'/></interface></node>";
    
    GDBusNodeInfo *app_node = g_dbus_node_info_new_for_xml(app_xml, NULL);
    GDBusNodeInfo *service_node = g_dbus_node_info_new_for_xml(service_xml, NULL);
    GDBusNodeInfo *char_node = g_dbus_node_info_new_for_xml(char_xml, NULL);
    GDBusNodeInfo *advert_node = g_dbus_node_info_new_for_xml(advert_xml, NULL);
    
    g_dbus_connection_register_object(connection, APP_PATH, app_node->interfaces[0], &app_vtable, NULL, NULL, NULL);
    g_dbus_connection_register_object(connection, SERVICE_PATH, service_node->interfaces[0], &service_vtable, NULL, NULL, NULL);
    g_dbus_connection_register_object(connection, CHAR_PATH, char_node->interfaces[0], &char_vtable, NULL, NULL, NULL);
    g_dbus_connection_register_object(connection, ADVERT_PATH, advert_node->interfaces[0], &advert_vtable, NULL, NULL, NULL);
    
    g_dbus_connection_call_sync(connection, "org.bluez", "/org/bluez/hci0", "org.bluez.GattManager1",
        "RegisterApplication", g_variant_new("(oa{sv})", APP_PATH, NULL), NULL, G_DBUS_CALL_FLAGS_NONE, -1, NULL, NULL);
    
    g_dbus_connection_call_sync(connection, "org.bluez", "/org/bluez/hci0", "org.bluez.LEAdvertisingManager1",
        "RegisterAdvertisement", g_variant_new("(oa{sv})", ADVERT_PATH, NULL), NULL, G_DBUS_CALL_FLAGS_NONE, -1, NULL, NULL);
    
    printf("Advertising as 'BLE-Notify'\n");
    printf("Service: %s\n\n", SERVICE_UUID);
    
    g_timeout_add_seconds(2, update_counter, NULL);
    
    main_loop = g_main_loop_new(NULL, FALSE);
    g_main_loop_run(main_loop);
    
    g_main_loop_unref(main_loop);
    g_object_unref(connection);
    
    return 0;
}
