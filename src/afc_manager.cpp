#include "afc_manager.h"
#include <iostream>
#include <fstream>
#include <cstring>

/*****************************************************************************/
/* Function Name: afc_manager (Constructor)                                  */
/*                                                                           */
/* Description: Initializes the AFC manager with null pointers               */
/*                                                                           */
/* Date            Engineer              Comments                            */
/* ------------------------------------------------------------------------- */
/* 2026-02-12      S. Amalfitano         Initial implementation              */
/*****************************************************************************/
afc_manager::afc_manager()
    : device(nullptr), lockdown_client(nullptr), afc_client(nullptr),
      service(nullptr), afc_connected(false)
{
}

/*****************************************************************************/
/* Function Name: ~afc_manager (Destructor)                                  */
/*                                                                           */
/* Description: Cleans up AFC connection on object destruction               */
/*                                                                           */
/* Date            Engineer              Comments                            */
/* ------------------------------------------------------------------------- */
/* 2026-02-12      S. Amalfitano         Initial implementation              */
/*****************************************************************************/
afc_manager::~afc_manager()
{
    disconnect();
}

/*****************************************************************************/
/* Function Name: connect_afc                                                */
/*                                                                           */
/* Description: Establishes AFC (Apple File Conduit) connection for file     */
/*              system access                                                */
/*                                                                           */
/* Date            Engineer              Comments                            */
/* ------------------------------------------------------------------------- */
/* 2026-02-12      S. Amalfitano         Initial implementation              */
/*****************************************************************************/
bool afc_manager::connect_afc(idevice_t dev, lockdownd_client_t lockdown)
{
    if (afc_connected)
    {
        std::cout << "AFC already connected." << std::endl;
        return true;
    }

    if (!dev || !lockdown)
    {
        std::cerr << "Error: Invalid device or lockdown client." << std::endl;
        return false;
    }

    device = dev;
    lockdown_client = lockdown;

    // Start AFC service
    lockdownd_error_t ldret = lockdownd_start_service(lockdown_client, "com.apple.afc", &service);
    if (ldret != LOCKDOWN_E_SUCCESS)
    {
        std::cerr << "Error: Failed to start AFC service." << std::endl;
        return false;
    }

    // Create AFC client
    afc_error_t afcret = afc_client_new(device, service, &afc_client);
    if (afcret != AFC_E_SUCCESS)
    {
        std::cerr << "Error: Failed to create AFC client." << std::endl;
        if (service)
        {
            lockdownd_service_descriptor_free(service);
            service = nullptr;
        }
        return false;
    }

    afc_connected = true;
    std::cout << "AFC connection established." << std::endl;
    return true;
}

/*****************************************************************************/
/* Function Name: disconnect                                                 */
/*                                                                           */
/* Description: Disconnects from AFC service and frees resources             */
/*                                                                           */
/* Date            Engineer              Comments                            */
/* ------------------------------------------------------------------------- */
/* 2026-02-12      S. Amalfitano         Initial implementation              */
/*****************************************************************************/
void afc_manager::disconnect()
{
    if (afc_client)
    {
        afc_client_free(afc_client);
        afc_client = nullptr;
    }

    if (service)
    {
        lockdownd_service_descriptor_free(service);
        service = nullptr;
    }

    afc_connected = false;
}

/*****************************************************************************/
/* Function Name: list_directory                                             */
/*                                                                           */
/* Description: Lists all files and directories in the specified path        */
/*                                                                           */
/* Date            Engineer              Comments                            */
/* ------------------------------------------------------------------------- */
/* 2026-02-12      S. Amalfitano         Initial implementation              */
/*****************************************************************************/
std::vector<std::string> afc_manager::list_directory(const std::string& path)
{
    std::vector<std::string> result;

    if (!afc_connected)
    {
        std::cerr << "Error: AFC not connected." << std::endl;
        return result;
    }

    char** list = nullptr;
    afc_error_t ret = afc_read_directory(afc_client, path.c_str(), &list);

    if (ret != AFC_E_SUCCESS)
    {
        std::cerr << "Error: Failed to read directory: " << path << std::endl;
        return result;
    }

    if (list)
    {
        for (int i = 0; list[i]; i++)
        {
            std::string entry = list[i];
            // Skip current and parent directory entries
            if (entry != "." && entry != "..")
            {
                result.push_back(entry);
            }
        }
        afc_dictionary_free(list);
    }

    return result;
}

/*****************************************************************************/
/* Function Name: create_directory                                           */
/*                                                                           */
/* Description: Creates a new directory on the device                        */
/*                                                                           */
/* Date            Engineer              Comments                            */
/* ------------------------------------------------------------------------- */
/* 2026-02-12      S. Amalfitano         Initial implementation              */
/*****************************************************************************/
bool afc_manager::create_directory(const std::string& path)
{
    if (!afc_connected)
    {
        std::cerr << "Error: AFC not connected." << std::endl;
        return false;
    }

    afc_error_t ret = afc_make_directory(afc_client, path.c_str());
    if (ret != AFC_E_SUCCESS)
    {
        std::cerr << "Error: Failed to create directory: " << path << std::endl;
        return false;
    }

    return true;
}

/*****************************************************************************/
/* Function Name: remove_path                                                */
/*                                                                           */
/* Description: Removes a file or directory from the device                  */
/*                                                                           */
/* Date            Engineer              Comments                            */
/* ------------------------------------------------------------------------- */
/* 2026-02-12      S. Amalfitano         Initial implementation              */
/*****************************************************************************/
bool afc_manager::remove_path(const std::string& path)
{
    if (!afc_connected)
    {
        std::cerr << "Error: AFC not connected." << std::endl;
        return false;
    }

    afc_error_t ret = afc_remove_path(afc_client, path.c_str());
    if (ret != AFC_E_SUCCESS)
    {
        std::cerr << "Error: Failed to remove path: " << path << std::endl;
        return false;
    }

    return true;
}

/*****************************************************************************/
/* Function Name: download_file                                              */
/*                                                                           */
/* Description: Downloads a file from the device to local filesystem         */
/*                                                                           */
/* Date            Engineer              Comments                            */
/* ------------------------------------------------------------------------- */
/* 2026-02-12      S. Amalfitano         Initial implementation              */
/*****************************************************************************/
bool afc_manager::download_file(const std::string& source_path, const std::string& destination_path)
{
    if (!afc_connected)
    {
        std::cerr << "Error: AFC not connected." << std::endl;
        return false;
    }

    // Open remote file for reading
    uint64_t handle = 0;
    afc_error_t ret = afc_file_open(afc_client, source_path.c_str(), AFC_FOPEN_RDONLY, &handle);
    if (ret != AFC_E_SUCCESS)
    {
        std::cerr << "Error: Failed to open remote file: " << source_path << std::endl;
        return false;
    }

    // Open local file for writing
    std::ofstream outfile(destination_path, std::ios::binary);
    if (!outfile.is_open())
    {
        std::cerr << "Error: Failed to create local file: " << destination_path << std::endl;
        afc_file_close(afc_client, handle);
        return false;
    }

    // Read and write in chunks
    const uint32_t chunk_size = 8192;
    char buffer[chunk_size];
    uint32_t bytes_read = 0;
    bool success = true;

    while (true)
    {
        ret = afc_file_read(afc_client, handle, buffer, chunk_size, &bytes_read);
        if (ret != AFC_E_SUCCESS)
        {
            std::cerr << "Error: Failed to read from remote file." << std::endl;
            success = false;
            break;
        }

        if (bytes_read == 0)
        {
            break;  // End of file
        }

        outfile.write(buffer, bytes_read);
        if (!outfile.good())
        {
            std::cerr << "Error: Failed to write to local file." << std::endl;
            success = false;
            break;
        }
    }

    afc_file_close(afc_client, handle);
    outfile.close();

    return success;
}

/*****************************************************************************/
/* Function Name: upload_file                                                */
/*                                                                           */
/* Description: Uploads a file from local filesystem to the device           */
/*                                                                           */
/* Date            Engineer              Comments                            */
/* ------------------------------------------------------------------------- */
/* 2026-02-12      S. Amalfitano         Initial implementation              */
/*****************************************************************************/
bool afc_manager::upload_file(const std::string& source_path, const std::string& destination_path)
{
    if (!afc_connected)
    {
        std::cerr << "Error: AFC not connected." << std::endl;
        return false;
    }

    // Open local file for reading
    std::ifstream infile(source_path, std::ios::binary);
    if (!infile.is_open())
    {
        std::cerr << "Error: Failed to open local file: " << source_path << std::endl;
        return false;
    }

    // Open remote file for writing
    uint64_t handle = 0;
    afc_error_t ret = afc_file_open(afc_client, destination_path.c_str(), AFC_FOPEN_WR, &handle);
    if (ret != AFC_E_SUCCESS)
    {
        std::cerr << "Error: Failed to create remote file: " << destination_path << std::endl;
        infile.close();
        return false;
    }

    // Read and write in chunks
    const uint32_t chunk_size = 8192;
    char buffer[chunk_size];
    bool success = true;

    while (infile.read(buffer, chunk_size) || infile.gcount() > 0)
    {
        uint32_t bytes_to_write = infile.gcount();
        uint32_t bytes_written = 0;

        ret = afc_file_write(afc_client, handle, buffer, bytes_to_write, &bytes_written);
        if (ret != AFC_E_SUCCESS || bytes_written != bytes_to_write)
        {
            std::cerr << "Error: Failed to write to remote file." << std::endl;
            success = false;
            break;
        }
    }

    afc_file_close(afc_client, handle);
    infile.close();

    return success;
}

/*****************************************************************************/
/* Function Name: file_exists                                                */
/*                                                                           */
/* Description: Checks if a file or directory exists on the device           */
/*                                                                           */
/* Date            Engineer              Comments                            */
/* ------------------------------------------------------------------------- */
/* 2026-02-12      S. Amalfitano         Initial implementation              */
/*****************************************************************************/
bool afc_manager::file_exists(const std::string& path)
{
    if (!afc_connected)
    {
        return false;
    }

    char** info = nullptr;
    afc_error_t ret = afc_get_file_info(afc_client, path.c_str(), &info);

    if (ret == AFC_E_SUCCESS && info)
    {
        afc_dictionary_free(info);
        return true;
    }

    return false;
}

/*****************************************************************************/
/* Function Name: get_file_info                                              */
/*                                                                           */
/* Description: Retrieves detailed information about a file or directory     */
/*                                                                           */
/* Date            Engineer              Comments                            */
/* ------------------------------------------------------------------------- */
/* 2026-02-12      S. Amalfitano         Initial implementation              */
/*****************************************************************************/
file_info afc_manager::get_file_info(const std::string& path)
{
    file_info info;
    info.full_path = path;
    info.filename = path.substr(path.find_last_of("/\\") + 1);
    info.file_size = 0;
    info.is_directory = false;

    if (!afc_connected)
    {
        return info;
    }

    char** file_info_list = nullptr;
    afc_error_t ret = afc_get_file_info(afc_client, path.c_str(), &file_info_list);

    if (ret == AFC_E_SUCCESS && file_info_list)
    {
        return parse_file_info(path, file_info_list);
    }

    return info;
}

/*****************************************************************************/
/* Function Name: parse_file_info                                            */
/*                                                                           */
/* Description: Parses file information dictionary from AFC                  */
/*                                                                           */
/* Date            Engineer              Comments                            */
/* ------------------------------------------------------------------------- */
/* 2026-02-12      S. Amalfitano         Initial implementation              */
/*****************************************************************************/
file_info afc_manager::parse_file_info(const std::string& path, char** file_info_list)
{
    file_info info;
    info.full_path = path;
    info.filename = path.substr(path.find_last_of("/\\") + 1);
    info.file_size = 0;
    info.is_directory = false;

    if (file_info_list)
    {
        for (int i = 0; file_info_list[i]; i += 2)
        {
            std::string key = file_info_list[i];
            std::string value = file_info_list[i + 1];

            if (key == "st_size")
            {
                info.file_size = std::stoull(value);
            }
            else if (key == "st_mtime")
            {
                info.modified_time = value;
            }
            else if (key == "st_ifmt")
            {
                info.is_directory = (value == "S_IFDIR");
            }
        }
        afc_dictionary_free(file_info_list);
    }

    return info;
}

/*****************************************************************************/
/* Function Name: format_file_size                                           */
/*                                                                           */
/* Description: Formats file size in human-readable format                   */
/*                                                                           */
/* Date            Engineer              Comments                            */
/* ------------------------------------------------------------------------- */
/* 2026-02-12      S. Amalfitano         Initial implementation              */
/*****************************************************************************/
std::string afc_manager::format_file_size(uint64_t size)
{
    const char* units[] = {"B", "KB", "MB", "GB", "TB"};
    int unit_index = 0;
    double display_size = static_cast<double>(size);

    while (display_size >= 1024.0 && unit_index < 4)
    {
        display_size /= 1024.0;
        unit_index++;
    }

    char buffer[64];
    snprintf(buffer, sizeof(buffer), "%.2f %s", display_size, units[unit_index]);
    return std::string(buffer);
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
bool afc_manager::is_connected() const
{
    return afc_connected;
}

/*****************************************************************************/
/* Function Name: print_file_list                                            */
/*                                                                           */
/* Description: Prints a list of files to console                            */
/*                                                                           */
/* Date            Engineer              Comments                            */
/* ------------------------------------------------------------------------- */
/* 2026-02-12      S. Amalfitano         Initial implementation              */
/*****************************************************************************/
void afc_manager::print_file_list(const std::vector<std::string>& files)
{
    std::cout << "\n=== File List (" << files.size() << " items) ===" << std::endl;
    for (const auto& file : files)
    {
        std::cout << "  " << file << std::endl;
    }
    std::cout << "=========================" << std::endl;
}

/*****************************************************************************/
/* Function Name: get_afc_client                                             */
/*                                                                           */
/* Description: Returns the AFC client handle for advanced operations        */
/*                                                                           */
/* Date            Engineer              Comments                            */
/* ------------------------------------------------------------------------- */
/* 2026-02-12      S. Amalfitano         Initial implementation              */
/*****************************************************************************/
afc_client_t afc_manager::get_afc_client() const
{
    return afc_client;
}
