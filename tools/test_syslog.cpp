#include <iostream>
#include <fstream>
#include <thread>
#include <chrono>
#include <csignal>
#include <atomic>
#include <cstring>
#include "../include/device_manager.h"
#include "../include/syslog_manager.h"

// Global flag for signal handling
std::atomic<bool> keep_running(true);

// Global file stream for logging
std::ofstream log_file;

/*****************************************************************************/
/* Function Name: signal_handler                                            */
/*                                                                           */
/* Description: Handles SIGINT (Ctrl+C) to gracefully stop the program     */
/*                                                                           */
/* Date            Engineer              Comments                            */
/* ------------------------------------------------------------------------- */
/* 2026-02-12      S. Amalfitano         Initial implementation              */
/*****************************************************************************/
void signal_handler(int signal)
{
    if (signal == SIGINT)
    {
        std::cout << "\n\nReceived Ctrl+C, stopping capture..." << std::endl;
        keep_running = false;

        // Close log file if open
        if (log_file.is_open())
        {
            log_file.close();
        }
    }
}

/*****************************************************************************/
/* Function Name: print_usage                                               */
/*                                                                           */
/* Description: Displays command-line usage information                     */
/*                                                                           */
/* Date            Engineer              Comments                            */
/* ------------------------------------------------------------------------- */
/* 2026-02-12      S. Amalfitano         Initial implementation              */
/*****************************************************************************/
void print_usage(const char* program_name)
{
    std::cout << "Usage: " << program_name << " [OPTIONS]\n" << std::endl;
    std::cout << "OPTIONS:" << std::endl;
    std::cout << "  -o, --output FILE    Save syslog output to FILE" << std::endl;
    std::cout << "  -h, --help           Display this help message" << std::endl;
    std::cout << "\nExamples:" << std::endl;
    std::cout << "  " << program_name << "                    # Display logs to console only" << std::endl;
    std::cout << "  " << program_name << " -o device.log      # Save logs to device.log" << std::endl;
}

/*****************************************************************************/
/* Function Name: main                                                       */
/*                                                                           */
/* Description: Test program for syslog_manager. Connects to device and     */
/*              streams system logs to console with graceful shutdown       */
/*                                                                           */
/* Date            Engineer              Comments                            */
/* ------------------------------------------------------------------------- */
/* 2026-02-12      S. Amalfitano         Initial implementation              */
/*****************************************************************************/
int main(int argc, char* argv[])
{
    std::string output_file;
    bool save_to_file = false;

    // Parse command-line arguments
    for (int i = 1; i < argc; i++)
    {
        if (strcmp(argv[i], "-o") == 0 || strcmp(argv[i], "--output") == 0)
        {
            if (i + 1 < argc)
            {
                output_file = argv[i + 1];
                save_to_file = true;
                i++; // Skip next argument
            }
            else
            {
                std::cerr << "Error: -o/--output requires a filename" << std::endl;
                return 1;
            }
        }
        else if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0)
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

    std::cout << "iOS Syslog Capture Tool" << std::endl;
    std::cout << "=======================" << std::endl;

    // Open log file if specified
    if (save_to_file)
    {
        log_file.open(output_file, std::ios::out | std::ios::app);
        if (!log_file.is_open())
        {
            std::cerr << "Error: Could not open file '" << output_file << "' for writing" << std::endl;
            return 1;
        }
        std::cout << "Logging to file: " << output_file << std::endl;
    }

    // Register signal handler for Ctrl+C
    std::signal(SIGINT, signal_handler);

    // Connect to device
    device_manager device;
    if (!device.connect_device())
    {
        return 1;
    }

    if (!device.connect_lockdown())
    {
        return 1;
    }

    // Show device info
    device.print_device_info();

    // Create syslog manager
    syslog_manager syslog(device.get_device());

    // Connect to syslog service
    if (!syslog.connect_syslog())
    {
        return 1;
    }

    // Start capturing logs
    std::cout << "\nStarting syslog capture..." << std::endl;
    std::cout << "Press Ctrl+C to stop\n" << std::endl;

    bool capture_started = syslog.start_capture([save_to_file](const std::string& line) {
        // Always print to console
        std::cout << line << std::endl;

        // Also write to file if enabled
        if (save_to_file && log_file.is_open())
        {
            log_file << line << std::endl;
            log_file.flush(); // Ensure data is written immediately
        }
    });

    if (!capture_started)
    {
        if (log_file.is_open())
        {
            log_file.close();
        }
        return 1;
    }

    // Keep the program running until Ctrl+C is pressed
    while (keep_running)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    // Graceful shutdown
    std::cout << "Stopping syslog capture..." << std::endl;
    syslog.stop_capture();
    std::cout << "Disconnecting..." << std::endl;
    syslog.disconnect();

    // Close log file if open
    if (log_file.is_open())
    {
        log_file.close();
        std::cout << "Log file closed: " << output_file << std::endl;
    }

    std::cout << "Cleanup complete. Exiting." << std::endl;

    return 0;
}
