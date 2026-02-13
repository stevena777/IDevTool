#ifndef DEVICE_MANAGER_H
#define DEVICE_MANAGER_H

#include <string>
#include <libimobiledevice/libimobiledevice.h>
#include <libimobiledevice/lockdown.h>
#include <plist/plist.h>

class device_manager {
private:
    idevice_t device;
    lockdownd_client_t client;
    bool device_connected;
    bool lockdown_connected;

    std::string get_string_value(const char* key);

public:
    device_manager();
    ~device_manager();

    // Connection methods
    bool connect_device();
    bool connect_lockdown();
    void disconnect();

    // Device info retrieval methods
    std::string get_device_name();
    std::string get_serial_number();
    std::string get_product_version();
    std::string get_product_type();
    std::string get_build_version();
    std::string get_activation_state();
    std::string get_unique_device_id();

    // Utility methods
    void print_device_info();
    bool is_connected() const;
    idevice_t get_device() const;
    lockdownd_client_t get_lockdown_client() const;
};

#endif // DEVICE_MANAGER_H
