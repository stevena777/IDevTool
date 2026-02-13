#ifndef SYSLOG_MANAGER_H
#define SYSLOG_MANAGER_H

#include <string>
#include <functional>
#include <libimobiledevice/libimobiledevice.h>
#include <libimobiledevice/syslog_relay.h>

class syslog_manager {
private:
    idevice_t device;
    syslog_relay_client_t syslog_client;
    bool syslog_connected;
    bool is_capturing;

    // Static callback wrapper for C API
    static void syslog_callback_wrapper(char c, void* user_data);

    // User callback function
    std::function<void(const std::string&)> line_callback;
    std::string current_line;

public:
    syslog_manager(idevice_t dev);
    ~syslog_manager();

    // Connection methods
    bool connect_syslog();
    void disconnect();

    // Syslog capture methods
    bool start_capture(std::function<void(const std::string&)> callback);
    bool stop_capture();

    // Utility methods
    bool is_connected() const;
    bool is_capturing_logs() const;
};

#endif // SYSLOG_MANAGER_H
