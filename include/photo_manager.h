#ifndef PHOTO_MANAGER_H
#define PHOTO_MANAGER_H

#include <string>
#include <vector>
#include "afc_manager.h"

struct photo_info {
    std::string filename;
    std::string full_path;
    uint64_t file_size;
    std::string modified_time;
    std::string file_type;  // jpg, png, heic, etc.
};

class photo_manager {
private:
    afc_manager* afc;
    bool owns_afc;  // Track if we created the afc_manager

    // Helper methods
    bool is_photo_file(const std::string& filename);
    bool is_video_file(const std::string& filename);
    std::string get_file_extension(const std::string& filename);
    void scan_for_photos(const std::string& path, std::vector<photo_info>& photos);
    photo_info file_info_to_photo_info(const file_info& finfo);

public:
    photo_manager();
    photo_manager(afc_manager* existing_afc);  // Use existing AFC connection
    ~photo_manager();

    // Connection methods
    bool connect(idevice_t dev, lockdownd_client_t lockdown);
    void disconnect();

    // Photo listing operations
    std::vector<photo_info> list_all_photos();
    std::vector<photo_info> list_photos_in_folder(const std::string& folder_path);
    std::vector<photo_info> list_videos();

    // Photo operations
    bool download_photo(const std::string& photo_path, const std::string& destination);
    bool download_all_photos(const std::string& destination_folder);
    int get_photo_count();
    int get_video_count();

    // Utility methods
    void print_photo_list(const std::vector<photo_info>& photos);
    bool is_connected() const;
};

#endif // PHOTO_MANAGER_H
