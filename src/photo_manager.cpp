#include "photo_manager.h"
#include <iostream>
#include <algorithm>

/*****************************************************************************/
/* Function Name: photo_manager (Constructor)                                */
/*                                                                           */
/* Description: Initializes the photo manager and creates new AFC manager    */
/*                                                                           */
/* Date            Engineer              Comments                            */
/* ------------------------------------------------------------------------- */
/* 2026-02-12      S. Amalfitano         Initial implementation              */
/*****************************************************************************/
photo_manager::photo_manager()
    : afc(new afc_manager()), owns_afc(true)
{
}

/*****************************************************************************/
/* Function Name: photo_manager (Constructor with existing AFC)              */
/*                                                                           */
/* Description: Initializes the photo manager with an existing AFC manager   */
/*                                                                           */
/* Date            Engineer              Comments                            */
/* ------------------------------------------------------------------------- */
/* 2026-02-12      S. Amalfitano         Initial implementation              */
/*****************************************************************************/
photo_manager::photo_manager(afc_manager* existing_afc)
    : afc(existing_afc), owns_afc(false)
{
}

/*****************************************************************************/
/* Function Name: ~photo_manager (Destructor)                                */
/*                                                                           */
/* Description: Cleans up photo manager and AFC if owned                     */
/*                                                                           */
/* Date            Engineer              Comments                            */
/* ------------------------------------------------------------------------- */
/* 2026-02-12      S. Amalfitano         Initial implementation              */
/*****************************************************************************/
photo_manager::~photo_manager()
{
    if (owns_afc && afc)
    {
        delete afc;
    }
}

/*****************************************************************************/
/* Function Name: connect                                                    */
/*                                                                           */
/* Description: Establishes AFC connection for photo access                  */
/*                                                                           */
/* Date            Engineer              Comments                            */
/* ------------------------------------------------------------------------- */
/* 2026-02-12      S. Amalfitano         Initial implementation              */
/*****************************************************************************/
bool photo_manager::connect(idevice_t dev, lockdownd_client_t lockdown)
{
    if (!afc)
    {
        std::cerr << "Error: AFC manager not initialized." << std::endl;
        return false;
    }

    return afc->connect_afc(dev, lockdown);
}

/*****************************************************************************/
/* Function Name: disconnect                                                 */
/*                                                                           */
/* Description: Disconnects from AFC service                                 */
/*                                                                           */
/* Date            Engineer              Comments                            */
/* ------------------------------------------------------------------------- */
/* 2026-02-12      S. Amalfitano         Initial implementation              */
/*****************************************************************************/
void photo_manager::disconnect()
{
    if (afc && owns_afc)
    {
        afc->disconnect();
    }
}

/*****************************************************************************/
/* Function Name: is_photo_file                                              */
/*                                                                           */
/* Description: Checks if a filename is a photo file based on extension      */
/*                                                                           */
/* Date            Engineer              Comments                            */
/* ------------------------------------------------------------------------- */
/* 2026-02-12      S. Amalfitano         Initial implementation              */
/*****************************************************************************/
bool photo_manager::is_photo_file(const std::string& filename)
{
    std::string ext = get_file_extension(filename);
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);

    return (ext == "jpg" || ext == "jpeg" || ext == "png" ||
            ext == "heic" || ext == "heif" || ext == "gif" ||
            ext == "bmp" || ext == "tiff" || ext == "tif");
}

/*****************************************************************************/
/* Function Name: is_video_file                                              */
/*                                                                           */
/* Description: Checks if a filename is a video file based on extension      */
/*                                                                           */
/* Date            Engineer              Comments                            */
/* ------------------------------------------------------------------------- */
/* 2026-02-12      S. Amalfitano         Initial implementation              */
/*****************************************************************************/
bool photo_manager::is_video_file(const std::string& filename)
{
    std::string ext = get_file_extension(filename);
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);

    return (ext == "mov" || ext == "mp4" || ext == "m4v" ||
            ext == "avi" || ext == "mkv");
}

/*****************************************************************************/
/* Function Name: get_file_extension                                         */
/*                                                                           */
/* Description: Extracts file extension from filename                        */
/*                                                                           */
/* Date            Engineer              Comments                            */
/* ------------------------------------------------------------------------- */
/* 2026-02-12      S. Amalfitano         Initial implementation              */
/*****************************************************************************/
std::string photo_manager::get_file_extension(const std::string& filename)
{
    size_t dot_pos = filename.find_last_of('.');
    if (dot_pos != std::string::npos && dot_pos < filename.length() - 1)
    {
        return filename.substr(dot_pos + 1);
    }
    return "";
}

/*****************************************************************************/
/* Function Name: scan_for_photos                                            */
/*                                                                           */
/* Description: Recursively scans a directory for photo files                */
/*                                                                           */
/* Date            Engineer              Comments                            */
/* ------------------------------------------------------------------------- */
/* 2026-02-12      S. Amalfitano         Initial implementation              */
/*****************************************************************************/
void photo_manager::scan_for_photos(const std::string& path, std::vector<photo_info>& photos)
{
    if (!afc || !afc->is_connected())
    {
        return;
    }

    std::vector<std::string> entries = afc->list_directory(path);

    for (const auto& entry : entries)
    {
        std::string full_path = path;
        if (full_path.back() != '/')
        {
            full_path += "/";
        }
        full_path += entry;

        file_info finfo = afc->get_file_info(full_path);

        if (finfo.is_directory)
        {
            // Recursively scan subdirectories
            scan_for_photos(full_path, photos);
        }
        else if (is_photo_file(entry))
        {
            // Add photo to list
            photos.push_back(file_info_to_photo_info(finfo));
        }
    }
}

/*****************************************************************************/
/* Function Name: file_info_to_photo_info                                    */
/*                                                                           */
/* Description: Converts file_info structure to photo_info structure         */
/*                                                                           */
/* Date            Engineer              Comments                            */
/* ------------------------------------------------------------------------- */
/* 2026-02-12      S. Amalfitano         Initial implementation              */
/*****************************************************************************/
photo_info photo_manager::file_info_to_photo_info(const file_info& finfo)
{
    photo_info pinfo;
    pinfo.filename = finfo.filename;
    pinfo.full_path = finfo.full_path;
    pinfo.file_size = finfo.file_size;
    pinfo.modified_time = finfo.modified_time;
    pinfo.file_type = get_file_extension(finfo.filename);

    return pinfo;
}

/*****************************************************************************/
/* Function Name: list_all_photos                                            */
/*                                                                           */
/* Description: Lists all photos from the device's DCIM folder               */
/*                                                                           */
/* Date            Engineer              Comments                            */
/* ------------------------------------------------------------------------- */
/* 2026-02-12      S. Amalfitano         Initial implementation              */
/*****************************************************************************/
std::vector<photo_info> photo_manager::list_all_photos()
{
    std::vector<photo_info> photos;

    if (!afc || !afc->is_connected())
    {
        std::cerr << "Error: AFC not connected." << std::endl;
        return photos;
    }

    // iOS stores photos in /DCIM directory
    std::cout << "Scanning DCIM folder for photos..." << std::endl;
    scan_for_photos("/DCIM", photos);

    // Sort photos by filename for consistent ordering
    std::sort(photos.begin(), photos.end(),
        [](const photo_info& a, const photo_info& b) {
            return a.filename < b.filename;
        });

    std::cout << "Found " << photos.size() << " photos." << std::endl;
    return photos;
}

/*****************************************************************************/
/* Function Name: list_photos_in_folder                                      */
/*                                                                           */
/* Description: Lists photos in a specific folder                            */
/*                                                                           */
/* Date            Engineer              Comments                            */
/* ------------------------------------------------------------------------- */
/* 2026-02-12      S. Amalfitano         Initial implementation              */
/*****************************************************************************/
std::vector<photo_info> photo_manager::list_photos_in_folder(const std::string& folder_path)
{
    std::vector<photo_info> photos;

    if (!afc || !afc->is_connected())
    {
        std::cerr << "Error: AFC not connected." << std::endl;
        return photos;
    }

    std::cout << "Scanning " << folder_path << " for photos..." << std::endl;
    scan_for_photos(folder_path, photos);

    // Sort photos by filename for consistent ordering
    std::sort(photos.begin(), photos.end(),
        [](const photo_info& a, const photo_info& b) {
            return a.filename < b.filename;
        });

    std::cout << "Found " << photos.size() << " photos." << std::endl;
    return photos;
}

/*****************************************************************************/
/* Function Name: list_videos                                                */
/*                                                                           */
/* Description: Lists all video files from the device's DCIM folder          */
/*                                                                           */
/* Date            Engineer              Comments                            */
/* ------------------------------------------------------------------------- */
/* 2026-02-12      S. Amalfitano         Initial implementation              */
/*****************************************************************************/
std::vector<photo_info> photo_manager::list_videos()
{
    std::vector<photo_info> videos;

    if (!afc || !afc->is_connected())
    {
        std::cerr << "Error: AFC not connected." << std::endl;
        return videos;
    }

    std::cout << "Scanning DCIM folder for videos..." << std::endl;

    // Scan DCIM directory
    std::vector<std::string> entries = afc->list_directory("/DCIM");
    for (const auto& entry : entries)
    {
        std::string full_path = "/DCIM/" + entry;
        file_info finfo = afc->get_file_info(full_path);

        if (finfo.is_directory)
        {
            // Scan subdirectory
            std::vector<std::string> subentries = afc->list_directory(full_path);
            for (const auto& subentry : subentries)
            {
                if (is_video_file(subentry))
                {
                    std::string video_path = full_path + "/" + subentry;
                    file_info vinfo = afc->get_file_info(video_path);
                    photo_info pinfo = file_info_to_photo_info(vinfo);
                    videos.push_back(pinfo);
                }
            }
        }
        else if (is_video_file(entry))
        {
            photo_info pinfo = file_info_to_photo_info(finfo);
            videos.push_back(pinfo);
        }
    }

    // Sort videos by filename for consistent ordering
    std::sort(videos.begin(), videos.end(),
        [](const photo_info& a, const photo_info& b) {
            return a.filename < b.filename;
        });

    std::cout << "Found " << videos.size() << " videos." << std::endl;
    return videos;
}

/*****************************************************************************/
/* Function Name: download_photo                                             */
/*                                                                           */
/* Description: Downloads a single photo from the device                     */
/*                                                                           */
/* Date            Engineer              Comments                            */
/* ------------------------------------------------------------------------- */
/* 2026-02-12      S. Amalfitano         Initial implementation              */
/*****************************************************************************/
bool photo_manager::download_photo(const std::string& photo_path, const std::string& destination)
{
    if (!afc || !afc->is_connected())
    {
        std::cerr << "Error: AFC not connected." << std::endl;
        return false;
    }

    std::cout << "Downloading: " << photo_path << " -> " << destination << std::endl;
    return afc->download_file(photo_path, destination);
}

/*****************************************************************************/
/* Function Name: download_all_photos                                        */
/*                                                                           */
/* Description: Downloads all photos from the device to a local folder       */
/*                                                                           */
/* Date            Engineer              Comments                            */
/* ------------------------------------------------------------------------- */
/* 2026-02-12      S. Amalfitano         Initial implementation              */
/*****************************************************************************/
bool photo_manager::download_all_photos(const std::string& destination_folder)
{
    std::vector<photo_info> photos = list_all_photos();

    if (photos.empty())
    {
        std::cout << "No photos found to download." << std::endl;
        return true;
    }

    int success_count = 0;
    int fail_count = 0;

    for (const auto& photo : photos)
    {
        std::string dest_path = destination_folder;
        if (dest_path.back() != '/' && dest_path.back() != '\\')
        {
            dest_path += "/";
        }
        dest_path += photo.filename;

        if (download_photo(photo.full_path, dest_path))
        {
            success_count++;
        }
        else
        {
            fail_count++;
        }
    }

    std::cout << "\n=== Download Summary ===" << std::endl;
    std::cout << "Successful: " << success_count << std::endl;
    std::cout << "Failed: " << fail_count << std::endl;
    std::cout << "========================" << std::endl;

    return (fail_count == 0);
}

/*****************************************************************************/
/* Function Name: get_photo_count                                            */
/*                                                                           */
/* Description: Returns the total number of photos on the device             */
/*                                                                           */
/* Date            Engineer              Comments                            */
/* ------------------------------------------------------------------------- */
/* 2026-02-12      S. Amalfitano         Initial implementation              */
/*****************************************************************************/
int photo_manager::get_photo_count()
{
    return list_all_photos().size();
}

/*****************************************************************************/
/* Function Name: get_video_count                                            */
/*                                                                           */
/* Description: Returns the total number of videos on the device             */
/*                                                                           */
/* Date            Engineer              Comments                            */
/* ------------------------------------------------------------------------- */
/* 2026-02-12      S. Amalfitano         Initial implementation              */
/*****************************************************************************/
int photo_manager::get_video_count()
{
    return list_videos().size();
}

/*****************************************************************************/
/* Function Name: print_photo_list                                           */
/*                                                                           */
/* Description: Prints a formatted list of photos                            */
/*                                                                           */
/* Date            Engineer              Comments                            */
/* ------------------------------------------------------------------------- */
/* 2026-02-12      S. Amalfitano         Initial implementation              */
/*****************************************************************************/
void photo_manager::print_photo_list(const std::vector<photo_info>& photos)
{
    std::cout << "\n=== Photo List (" << photos.size() << " photos) ===" << std::endl;

    for (size_t i = 0; i < photos.size(); i++)
    {
        const auto& photo = photos[i];
        std::cout << "[" << (i + 1) << "] " << photo.filename << std::endl;
        std::cout << "    Path: " << photo.full_path << std::endl;
        std::cout << "    Size: " << photo.file_size << " bytes" << std::endl;
        std::cout << "    Type: " << photo.file_type << std::endl;
        if (!photo.modified_time.empty())
        {
            std::cout << "    Modified: " << photo.modified_time << std::endl;
        }
        std::cout << std::endl;
    }

    std::cout << "=========================" << std::endl;
}

/*****************************************************************************/
/* Function Name: is_connected                                               */
/*                                                                           */
/* Description: Returns true if AFC connection is established                */
/*                                                                           */
/* Date            Engineer              Comments                            */
/* ------------------------------------------------------------------------- */
/* 2026-02-12      S. Amalfitano         Initial implementation              */
/*****************************************************************************/
bool photo_manager::is_connected() const
{
    return afc && afc->is_connected();
}
