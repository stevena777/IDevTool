#include <iostream>
#include "device_manager.h"

/*****************************************************************************/
/* Function Name: main                                                       */
/*                                                                           */
/* Description: Entry point for iOS Device Security Tool. Connects to        */
/*              device, establishes lockdown connection, and displays        */
/*              device information                                           */
/*                                                                           */
/* Date            Engineer              Comments                            */
/* ------------------------------------------------------------------------- */
/* 2026-02-12      S. Amalfitano         Initial implementation              */
/*****************************************************************************/
int main()
{
    std::cout << "iOS Device Security Tool" << std::endl;
    std::cout << "========================" << std::endl;

    device_manager device;

    // Connect to device
    if (!device.connect_device())
    {
        return 1;
    }

    // Connect to lockdown service
    if (!device.connect_lockdown())
    {
        return 1;
    }

    // Print all device information
    device.print_device_info();

    // You can also get individual properties
    std::cout << "\nIndividual property example:" << std::endl;
    std::cout << "Device Name: " << device.get_device_name() << std::endl;
    std::cout << "Serial: " << device.get_serial_number() << std::endl;

    // Cleanup is automatic (destructor)
    return 0;
}
