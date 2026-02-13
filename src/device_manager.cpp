#include "device_manager.h"
#include <iostream>

/*****************************************************************************/
/* Function Name: device_manager (Constructor)                               */
/*                                                                           */
/* Description: Initializes the device manager with null device and client   */
/*              pointers                                                     */
/*                                                                           */
/* Date            Engineer              Comments                            */
/* ------------------------------------------------------------------------- */
/* 2026-02-12      S. Amalfitano         Initial implementation              */
/*****************************************************************************/
device_manager::device_manager()
    : device(nullptr), client(nullptr),
      device_connected(false), lockdown_connected(false)
{
}

/*****************************************************************************/
/* Function Name: ~device_manager (Destructor)                               */
/*                                                                           */
/* Description: Cleans up device and lockdown connections on object          */
/*              destruction                                                  */
/*                                                                           */
/* Date            Engineer              Comments                            */
/* ------------------------------------------------------------------------- */
/* 2026-02-12      S. Amalfitano         Initial implementation              */
/*****************************************************************************/
device_manager::~device_manager()
{
    disconnect();
}

/*****************************************************************************/
/* Function Name: connect_device                                             */
/*                                                                           */
/* Description: Connects to the first available iOS device                   */
/*                                                                           */
/* Date            Engineer              Comments                            */
/* ------------------------------------------------------------------------- */
/* 2026-02-12      S. Amalfitano         Initial implementation              */
/*****************************************************************************/
bool device_manager::connect_device()
{
    if (device_connected)
    {
        std::cout << "Device already connected." << std::endl;
        return true;
    }

    if (idevice_new(&device, NULL) != IDEVICE_E_SUCCESS)
    {
        std::cerr << "Error: No device found." << std::endl;
        return false;
    }

    device_connected = true;
    std::cout << "Device connected successfully." << std::endl;
    return true;
}

/*****************************************************************************/
/* Function Name: connect_lockdown                                           */
/*                                                                           */
/* Description: Establishes a lockdown service connection with the device    */
/*              for retrieving device information and performing handshake   */
/*                                                                           */
/* Date            Engineer              Comments                            */
/* ------------------------------------------------------------------------- */
/* 2026-02-12      S. Amalfitano         Initial implementation              */
/*****************************************************************************/
bool device_manager::connect_lockdown()
{
    if (!device_connected)
    {
        std::cerr << "Error: Device not connected. Call connect_device() first." << std::endl;
        return false;
    }

    if (lockdown_connected)
    {
        std::cout << "Lockdown already connected." << std::endl;
        return true;
    }

    if (lockdownd_client_new_with_handshake(device, &client, "security-tool")
        != LOCKDOWN_E_SUCCESS)
    {
        std::cerr << "Error: Failed to connect to lockdown service." << std::endl;
        return false;
    }

    lockdown_connected = true;
    std::cout << "Lockdown connection established." << std::endl;
    return true;
}

/*****************************************************************************/
/* Function Name: disconnect                                                 */
/*                                                                           */
/* Description: Disconnects from lockdown service and device, freeing all    */
/*              resources                                                    */
/*                                                                           */
/* Date            Engineer              Comments                            */
/* ------------------------------------------------------------------------- */
/* 2026-02-12      S. Amalfitano         Initial implementation              */
/*****************************************************************************/
void device_manager::disconnect()
{
    if (lockdown_connected && client)
    {
        lockdownd_client_free(client);
        client = nullptr;
        lockdown_connected = false;
    }

    if (device_connected && device)
    {
        idevice_free(device);
        device = nullptr;
        device_connected = false;
    }
}

/*****************************************************************************/
/* Function Name: get_string_value                                           */
/*                                                                           */
/* Description: Helper function to retrieve string values from lockdown      */
/*              service for a given key                                      */
/*                                                                           */
/* Date            Engineer              Comments                            */
/* ------------------------------------------------------------------------- */
/* 2026-02-12      S. Amalfitano         Initial implementation              */
/*****************************************************************************/
std::string device_manager::get_string_value(const char* key)
{
    if (!lockdown_connected)
    {
        return "<not connected>";
    }

    plist_t node = nullptr;

    if (lockdownd_get_value(client, NULL, key, &node) != LOCKDOWN_E_SUCCESS || !node)
    {
        return "<unavailable>";
    }

    std::string result;
    if (plist_get_node_type(node) == PLIST_STRING)
    {
        char* value = nullptr;
        plist_get_string_val(node, &value);
        if (value)
        {
            result = value;
        }
        else
        {
            result = "<null>";
        }
    }
    else
    {
        result = "<not a string>";
    }

    plist_free(node);
    return result;
}

/*****************************************************************************/
/* Function Name: get_device_name                                            */
/*                                                                           */
/* Description: Retrieves the device name from lockdown service              */
/*                                                                           */
/* Date            Engineer              Comments                            */
/* ------------------------------------------------------------------------- */
/* 2026-02-12      S. Amalfitano         Initial implementation              */
/*****************************************************************************/
std::string device_manager::get_device_name()
{
    return get_string_value("DeviceName");
}

/*****************************************************************************/
/* Function Name: get_serial_number                                          */
/*                                                                           */
/* Description: Retrieves the device serial number from lockdown service     */
/*                                                                           */
/* Date            Engineer              Comments                            */
/* ------------------------------------------------------------------------- */
/* 2026-02-12      S. Amalfitano         Initial implementation              */
/*****************************************************************************/
std::string device_manager::get_serial_number()
{
    return get_string_value("SerialNumber");
}

/*****************************************************************************/
/* Function Name: get_product_version                                        */
/*                                                                           */
/* Description: Retrieves the iOS product version from lockdown service      */
/*                                                                           */
/* Date            Engineer              Comments                            */
/* ------------------------------------------------------------------------- */
/* 2026-02-12      S. Amalfitano         Initial implementation              */
/*****************************************************************************/
std::string device_manager::get_product_version()
{
    return get_string_value("ProductVersion");
}

/*****************************************************************************/
/* Function Name: get_product_type                                           */
/*                                                                           */
/* Description: Retrieves the device product type from lockdown service      */
/*                                                                           */
/* Date            Engineer              Comments                            */
/* ------------------------------------------------------------------------- */
/* 2026-02-12      S. Amalfitano         Initial implementation              */
/*****************************************************************************/
std::string device_manager::get_product_type()
{
    return get_string_value("ProductType");
}

/*****************************************************************************/
/* Function Name: get_build_version                                          */
/*                                                                           */
/* Description: Retrieves the iOS build version from lockdown service        */
/*                                                                           */
/* Date            Engineer              Comments                            */
/* ------------------------------------------------------------------------- */
/* 2026-02-12      S. Amalfitano         Initial implementation              */
/*****************************************************************************/
std::string device_manager::get_build_version()
{
    return get_string_value("BuildVersion");
}

/*****************************************************************************/
/* Function Name: get_activation_state                                       */
/*                                                                           */
/* Description: Retrieves the device activation state from lockdown service  */
/*                                                                           */
/* Date            Engineer              Comments                            */
/* ------------------------------------------------------------------------- */
/* 2026-02-12      S. Amalfitano         Initial implementation              */
/*****************************************************************************/
std::string device_manager::get_activation_state()
{
    return get_string_value("ActivationState");
}

/*****************************************************************************/
/* Function Name: get_unique_device_id                                       */
/*                                                                           */
/* Description: Retrieves the unique device identifier (UDID) from lockdown  */
/*              service                                                      */
/*                                                                           */
/* Date            Engineer              Comments                            */
/* ------------------------------------------------------------------------- */
/* 2026-02-12      S. Amalfitano         Initial implementation              */
/*****************************************************************************/
std::string device_manager::get_unique_device_id()
{
    return get_string_value("UniqueDeviceID");
}

/*****************************************************************************/
/* Function Name: print_device_info                                          */
/*                                                                           */
/* Description: Prints all device information in a formatted display         */
/*                                                                           */
/* Date            Engineer              Comments                            */
/* ------------------------------------------------------------------------- */
/* 2026-02-12      S. Amalfitano         Initial implementation              */
/*****************************************************************************/
void device_manager::print_device_info()
{
    if (!lockdown_connected)
    {
        std::cerr << "Error: Not connected to lockdown service." << std::endl;
        return;
    }

    std::cout << "\n=== Device Information ===" << std::endl;
    std::cout << "Device Name:      " << get_device_name() << std::endl;
    std::cout << "Serial Number:    " << get_serial_number() << std::endl;
    std::cout << "Product Version:  " << get_product_version() << std::endl;
    std::cout << "Product Type:     " << get_product_type() << std::endl;
    std::cout << "Build Version:    " << get_build_version() << std::endl;
    std::cout << "Activation State: " << get_activation_state() << std::endl;
    std::cout << "Unique Device ID: " << get_unique_device_id() << std::endl;
    std::cout << "=========================" << std::endl;
}

/*****************************************************************************/
/* Function Name: is_connected                                               */
/*                                                                           */
/* Description: Returns true if both device and lockdown connections are     */
/*              established                                                  */
/*                                                                           */
/* Date            Engineer              Comments                            */
/* ------------------------------------------------------------------------- */
/* 2026-02-12      S. Amalfitano         Initial implementation              */
/*****************************************************************************/
bool device_manager::is_connected() const
{
    return device_connected && lockdown_connected;
}

/*****************************************************************************/
/* Function Name: get_device                                                 */
/*                                                                           */
/* Description: Returns the device handle for use by other managers          */
/*                                                                           */
/* Date            Engineer              Comments                            */
/* ------------------------------------------------------------------------- */
/* 2026-02-12      S. Amalfitano         Initial implementation              */
/*****************************************************************************/
idevice_t device_manager::get_device() const
{
    return device;
}
