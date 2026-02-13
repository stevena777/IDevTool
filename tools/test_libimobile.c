#include <stdio.h>
#include <stdlib.h>

#include <libimobiledevice/libimobiledevice.h>
#include <libimobiledevice/lockdown.h>
#include <plist/plist.h>

/* Safe helper: get a string value from lockdown */
void get_and_print_string(lockdownd_client_t client, const char *key)
{
    plist_t node = NULL;

    if (lockdownd_get_value(client, NULL, key, &node) != LOCKDOWN_E_SUCCESS || !node) {
        printf("%s: <unavailable>\n", key);
        return;
    }

    if (plist_get_node_type(node) == PLIST_STRING) {
        char *value = NULL;
        plist_get_string_val(node, &value);
        if (value) {
            printf("%s: %s\n", key, value);
            // Removed free(value) - plist_free handles this
        }
    } else {
        printf("%s: <not a string>\n", key);
    }

    plist_free(node);
}

int main(void)
{
    idevice_t device = NULL;
    lockdownd_client_t client = NULL;

    /* Connect to first available device */
    if (idevice_new(&device, NULL) != IDEVICE_E_SUCCESS) {
        printf("No device found.\n");
        return 1;
    }

    /* Handshake with lockdown */
    if (lockdownd_client_new_with_handshake(device, &client, "lockdown-test")
        != LOCKDOWN_E_SUCCESS) {
        printf("Failed to connect to lockdown.\n");
        idevice_free(device);
        return 1;
    }

    printf("Connected to device via lockdown\n\n");

    /* Retrieve device info */
    get_and_print_string(client, "DeviceName");
    get_and_print_string(client, "ProductVersion");
    get_and_print_string(client, "ProductType");
    get_and_print_string(client, "BuildVersion");
    get_and_print_string(client, "SerialNumber");
    get_and_print_string(client, "ActivationState");
    get_and_print_string(client, "UniqueDeviceID");

    /* Cleanup */
    lockdownd_client_free(client);
    idevice_free(device);

    return 0;
}