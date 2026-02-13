#include <iostream>
#include <string>
#include <limits>
#include <algorithm>
#include "device_manager.h"
#include "photo_manager.h"

/*****************************************************************************/
/* Function Name: print_usage                                                */
/*                                                                           */
/* Description: Displays command-line usage information                      */
/*                                                                           */
/* Date            Engineer              Comments                            */
/* ------------------------------------------------------------------------- */
/* 2026-02-12      S. Amalfitano         Initial implementation              */
/*****************************************************************************/
void print_usage(const char* program_name)
{
    std::cout << "Usage: " << program_name << " [OPTIONS]\n" << std::endl;
    std::cout << "OPTIONS:" << std::endl;
    std::cout << "  -l, --list           List all photos and exit" << std::endl;
    std::cout << "  -d, --download DIR   Download all photos to DIR and exit" << std::endl;
    std::cout << "  -s, --stats          Show photo statistics and exit" << std::endl;
    std::cout << "  -h, --help           Display this help message" << std::endl;
    std::cout << "\nInteractive Mode:" << std::endl;
    std::cout << "  Run without options to enter interactive menu" << std::endl;
    std::cout << "\nExamples:" << std::endl;
    std::cout << "  " << program_name << "                      # Interactive mode" << std::endl;
    std::cout << "  " << program_name << " -l                   # List all photos" << std::endl;
    std::cout << "  " << program_name << " -d ./my_photos       # Download all photos" << std::endl;
    std::cout << "  " << program_name << " -s                   # Show statistics" << std::endl;
}

/*****************************************************************************/
/* Function Name: print_menu                                                 */
/*                                                                           */
/* Description: Displays the interactive menu                                */
/*                                                                           */
/* Date            Engineer              Comments                            */
/* ------------------------------------------------------------------------- */
/* 2026-02-12      S. Amalfitano         Initial implementation              */
/*****************************************************************************/
void print_menu()
{
    std::cout << "\n=== Photo Manager Menu ===" << std::endl;
    std::cout << "1. List all photos" << std::endl;
    std::cout << "2. List all videos" << std::endl;
    std::cout << "3. Show photo/video statistics" << std::endl;
    std::cout << "4. Download specific photo" << std::endl;
    std::cout << "5. Download all photos" << std::endl;
    std::cout << "6. Browse DCIM folders" << std::endl;
    std::cout << "0. Exit" << std::endl;
    std::cout << "==========================" << std::endl;
    std::cout << "Enter choice: ";
}

/*****************************************************************************/
/* Function Name: list_photos_interactive                                    */
/*                                                                           */
/* Description: Lists photos and returns the list for further operations     */
/*                                                                           */
/* Date            Engineer              Comments                            */
/* ------------------------------------------------------------------------- */
/* 2026-02-12      S. Amalfitano         Initial implementation              */
/*****************************************************************************/
std::vector<photo_info> list_photos_interactive(photo_manager& photos)
{
    std::cout << "\nScanning for photos..." << std::endl;
    std::vector<photo_info> photo_list = photos.list_all_photos();

    if (photo_list.empty())
    {
        std::cout << "No photos found on device." << std::endl;
        return photo_list;
    }

    std::cout << "\n=== Photo List (" << photo_list.size() << " photos) ===" << std::endl;
    for (size_t i = 0; i < photo_list.size(); i++)
    {
        const auto& photo = photo_list[i];
        std::cout << "[" << (i + 1) << "] " << photo.filename;
        std::cout << " (" << photo.file_size << " bytes, " << photo.file_type << ")" << std::endl;
    }
    std::cout << "=========================" << std::endl;

    return photo_list;
}

/*****************************************************************************/
/* Function Name: list_videos_interactive                                    */
/*                                                                           */
/* Description: Lists videos from the device                                 */
/*                                                                           */
/* Date            Engineer              Comments                            */
/* ------------------------------------------------------------------------- */
/* 2026-02-12      S. Amalfitano         Initial implementation              */
/*****************************************************************************/
void list_videos_interactive(photo_manager& photos)
{
    std::cout << "\nScanning for videos..." << std::endl;
    std::vector<photo_info> video_list = photos.list_videos();

    if (video_list.empty())
    {
        std::cout << "No videos found on device." << std::endl;
        return;
    }

    std::cout << "\n=== Video List (" << video_list.size() << " videos) ===" << std::endl;
    for (size_t i = 0; i < video_list.size(); i++)
    {
        const auto& video = video_list[i];
        std::cout << "[" << (i + 1) << "] " << video.filename;
        std::cout << " (" << video.file_size << " bytes, " << video.file_type << ")" << std::endl;
    }
    std::cout << "=========================" << std::endl;
}

/*****************************************************************************/
/* Function Name: show_statistics                                            */
/*                                                                           */
/* Description: Displays photo and video statistics                          */
/*                                                                           */
/* Date            Engineer              Comments                            */
/* ------------------------------------------------------------------------- */
/* 2026-02-12      S. Amalfitano         Initial implementation              */
/*****************************************************************************/
void show_statistics(photo_manager& photos)
{
    std::cout << "\nGathering statistics..." << std::endl;

    std::vector<photo_info> photo_list = photos.list_all_photos();
    std::vector<photo_info> video_list = photos.list_videos();

    uint64_t total_photo_size = 0;
    uint64_t total_video_size = 0;

    for (const auto& photo : photo_list)
    {
        total_photo_size += photo.file_size;
    }

    for (const auto& video : video_list)
    {
        total_video_size += video.file_size;
    }

    std::cout << "\n=== Photo Library Statistics ===" << std::endl;
    std::cout << "Photos: " << photo_list.size() << std::endl;
    std::cout << "  Total size: " << (total_photo_size / 1024.0 / 1024.0) << " MB" << std::endl;
    std::cout << "Videos: " << video_list.size() << std::endl;
    std::cout << "  Total size: " << (total_video_size / 1024.0 / 1024.0) << " MB" << std::endl;
    std::cout << "Total items: " << (photo_list.size() + video_list.size()) << std::endl;
    std::cout << "Total size: " << ((total_photo_size + total_video_size) / 1024.0 / 1024.0) << " MB" << std::endl;
    std::cout << "================================" << std::endl;
}

/*****************************************************************************/
/* Function Name: download_specific_photo                                    */
/*                                                                           */
/* Description: Downloads a specific photo selected by the user              */
/*                                                                           */
/* Date            Engineer              Comments                            */
/* ------------------------------------------------------------------------- */
/* 2026-02-12      S. Amalfitano         Initial implementation              */
/*****************************************************************************/
void download_specific_photo(photo_manager& photos, const std::vector<photo_info>& photo_list)
{
    if (photo_list.empty())
    {
        std::cout << "\nNo photos available. Please list photos first (option 1)." << std::endl;
        return;
    }

    std::cout << "\nEnter photo number (1-" << photo_list.size() << "): ";
    int photo_num;
    std::cin >> photo_num;

    if (std::cin.fail() || photo_num < 1 || photo_num > (int)photo_list.size())
    {
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout << "Invalid photo number." << std::endl;
        return;
    }

    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    const auto& photo = photo_list[photo_num - 1];

    std::cout << "Enter destination filename (or press Enter for '" << photo.filename << "'): ";
    std::string dest_filename;
    std::getline(std::cin, dest_filename);

    if (dest_filename.empty())
    {
        dest_filename = photo.filename;
    }

    std::cout << "\nDownloading: " << photo.filename << " -> " << dest_filename << std::endl;

    if (photos.download_photo(photo.full_path, dest_filename))
    {
        std::cout << "Download successful!" << std::endl;
    }
    else
    {
        std::cout << "Download failed!" << std::endl;
    }
}

/*****************************************************************************/
/* Function Name: download_all_photos_interactive                            */
/*                                                                           */
/* Description: Downloads all photos to a specified directory                */
/*                                                                           */
/* Date            Engineer              Comments                            */
/* ------------------------------------------------------------------------- */
/* 2026-02-12      S. Amalfitano         Initial implementation              */
/*****************************************************************************/
void download_all_photos_interactive(photo_manager& photos)
{
    std::cout << "\nEnter destination directory (e.g., ./my_photos): ";
    std::string dest_dir;
    std::getline(std::cin, dest_dir);

    if (dest_dir.empty())
    {
        std::cout << "No directory specified. Cancelled." << std::endl;
        return;
    }

    std::cout << "\nDownloading all photos to: " << dest_dir << std::endl;
    std::cout << "This may take a while..." << std::endl;

    if (photos.download_all_photos(dest_dir))
    {
        std::cout << "\nAll photos downloaded successfully!" << std::endl;
    }
    else
    {
        std::cout << "\nSome photos failed to download." << std::endl;
    }
}

/*****************************************************************************/
/* Function Name: browse_folders                                             */
/*                                                                           */
/* Description: Browse DCIM folders on the device                            */
/*                                                                           */
/* Date            Engineer              Comments                            */
/* ------------------------------------------------------------------------- */
/* 2026-02-12      S. Amalfitano         Initial implementation              */
/*****************************************************************************/
void browse_folders(photo_manager& photos)
{
    std::cout << "\nEnter folder path to browse (e.g., /DCIM/100APPLE): ";
    std::string folder_path;
    std::getline(std::cin, folder_path);

    if (folder_path.empty())
    {
        folder_path = "/DCIM";
    }

    std::vector<photo_info> folder_photos = photos.list_photos_in_folder(folder_path);

    if (folder_photos.empty())
    {
        std::cout << "No photos found in " << folder_path << std::endl;
    }
    else
    {
        std::cout << "\n=== Photos in " << folder_path << " ===" << std::endl;
        for (size_t i = 0; i < folder_photos.size(); i++)
        {
            const auto& photo = folder_photos[i];
            std::cout << "[" << (i + 1) << "] " << photo.filename << std::endl;
        }
        std::cout << "=========================" << std::endl;
    }
}

/*****************************************************************************/
/* Function Name: interactive_mode                                           */
/*                                                                           */
/* Description: Runs the interactive menu loop                               */
/*                                                                           */
/* Date            Engineer              Comments                            */
/* ------------------------------------------------------------------------- */
/* 2026-02-12      S. Amalfitano         Initial implementation              */
/*****************************************************************************/
void interactive_mode(photo_manager& photos)
{
    std::vector<photo_info> cached_photo_list;
    bool running = true;

    while (running)
    {
        print_menu();

        int choice;
        std::cin >> choice;

        if (std::cin.fail())
        {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cout << "Invalid input. Please enter a number." << std::endl;
            continue;
        }

        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        switch (choice)
        {
            case 0:
                std::cout << "Exiting..." << std::endl;
                running = false;
                break;

            case 1:
                cached_photo_list = list_photos_interactive(photos);
                break;

            case 2:
                list_videos_interactive(photos);
                break;

            case 3:
                show_statistics(photos);
                break;

            case 4:
                download_specific_photo(photos, cached_photo_list);
                break;

            case 5:
                download_all_photos_interactive(photos);
                break;

            case 6:
                browse_folders(photos);
                break;

            default:
                std::cout << "Invalid choice. Please try again." << std::endl;
                break;
        }
    }
}

/*****************************************************************************/
/* Function Name: main                                                       */
/*                                                                           */
/* Description: Entry point for photo manager test. Supports both           */
/*              interactive mode and command-line options                    */
/*                                                                           */
/* Date            Engineer              Comments                            */
/* ------------------------------------------------------------------------- */
/* 2026-02-12      S. Amalfitano         Initial implementation              */
/*****************************************************************************/
int main(int argc, char* argv[])
{
    bool interactive = true;
    bool list_only = false;
    bool stats_only = false;
    std::string download_dir;

    // Parse command-line arguments
    for (int i = 1; i < argc; i++)
    {
        std::string arg = argv[i];

        if (arg == "-l" || arg == "--list")
        {
            list_only = true;
            interactive = false;
        }
        else if (arg == "-s" || arg == "--stats")
        {
            stats_only = true;
            interactive = false;
        }
        else if (arg == "-d" || arg == "--download")
        {
            if (i + 1 < argc)
            {
                download_dir = argv[i + 1];
                interactive = false;
                i++;
            }
            else
            {
                std::cerr << "Error: -d/--download requires a directory path" << std::endl;
                return 1;
            }
        }
        else if (arg == "-h" || arg == "--help")
        {
            print_usage(argv[0]);
            return 0;
        }
        else
        {
            std::cerr << "Error: Unknown option: " << argv[i] << std::endl;
            print_usage(argv[0]);
            return 1;
        }
    }

    std::cout << "iOS Photo Manager" << std::endl;
    std::cout << "=================" << std::endl;

    // Step 1: Connect to device
    device_manager device;
    if (!device.connect_device())
    {
        return 1;
    }

    if (!device.connect_lockdown())
    {
        return 1;
    }

    // Print device info
    std::cout << "\nConnected to: " << device.get_device_name() << std::endl;
    std::cout << "Product: " << device.get_product_type() << std::endl;
    std::cout << "iOS Version: " << device.get_product_version() << std::endl;

    // Step 2: Connect to photo manager
    photo_manager photos;
    std::cout << "\nConnecting to photo library..." << std::endl;

    if (!photos.connect(device.get_device(), device.get_lockdown_client()))
    {
        std::cerr << "Failed to connect to photo library." << std::endl;
        return 1;
    }

    // Execute based on mode
    if (list_only)
    {
        list_photos_interactive(photos);
    }
    else if (stats_only)
    {
        show_statistics(photos);
    }
    else if (!download_dir.empty())
    {
        std::cout << "\nDownloading all photos to: " << download_dir << std::endl;
        if (photos.download_all_photos(download_dir))
        {
            std::cout << "Download complete!" << std::endl;
        }
        else
        {
            std::cout << "Some downloads failed." << std::endl;
            return 1;
        }
    }
    else if (interactive)
    {
        interactive_mode(photos);
    }

    std::cout << "\nDisconnecting..." << std::endl;
    photos.disconnect();

    return 0;
}
