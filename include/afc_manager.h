#ifndef AFC_MANAGER_H
#define AFC_MANAGER_H

#include <string>
#include <vector>
#include <libimobiledevice/libimobiledevice.h>
#include <libimobiledevice/lockdown.h>
#include <libimobiledevice/afc.h>

struct file_info {
    std::string filename;
    std::string full_path;
    uint64_t file_size;
    std::string modified_time;
    bool is_directory;
};

class afc_manager {
private:
    idevice_t device;
    lockdownd_client_t lockdown_client;
    afc_client_t afc_client;
    lockdownd_service_descriptor_t service;
    bool afc_connected;

    // Helper methods
    std::string format_file_size(uint64_t size);
    file_info parse_file_info(const std::string& path, char** file_info_list);

public:
    afc_manager();
    ~afc_manager();

    // Connection methods
    bool connect_afc(idevice_t dev, lockdownd_client_t lockdown);
    void disconnect();

    // Directory operations
    std::vector<std::string> list_directory(const std::string& path);
    bool create_directory(const std::string& path);
    bool remove_path(const std::string& path);

    // File operations
    bool download_file(const std::string& source_path, const std::string& destination_path);
    bool upload_file(const std::string& source_path, const std::string& destination_path);
    bool file_exists(const std::string& path);
    file_info get_file_info(const std::string& path);

    // Utility methods
    bool is_connected() const;
    void print_file_list(const std::vector<std::string>& files);
    afc_client_t get_afc_client() const;
};

#endif // AFC_MANAGER_H
