#include "syslog_manager.h"
#include <iostream>

/*****************************************************************************/
/* Function Name: syslog_manager (Constructor)                               */
/*                                                                           */
/* Description: Initializes the syslog manager with an existing device       */
/*              connection                                                   */
/*                                                                           */
/* Date            Engineer              Comments                            */
/* ------------------------------------------------------------------------- */
/* 2026-02-12      S. Amalfitano         Initial implementation              */
/*****************************************************************************/
syslog_manager::syslog_manager(idevice_t dev)
    : device(dev), syslog_client(nullptr),
      syslog_connected(false), is_capturing(false)
{
}

/*****************************************************************************/
/* Function Name: ~syslog_manager (Destructor)                               */
/*                                                                           */
/* Description: Cleans up syslog connection on object destruction            */
/*                                                                           */
/* Date            Engineer              Comments                            */
/* ------------------------------------------------------------------------- */
/* 2026-02-12      S. Amalfitano         Initial implementation              */
/*****************************************************************************/
syslog_manager::~syslog_manager()
{
    disconnect();
}

/*****************************************************************************/
/* Function Name: connect_syslog                                             */
/*                                                                           */
/* Description: Establishes a syslog relay service connection with the       */
/*              device for capturing system logs                             */
/*                                                                           */
/* Date            Engineer              Comments                            */
/* ------------------------------------------------------------------------- */
/* 2026-02-12      S. Amalfitano         Initial implementation              */
/*****************************************************************************/
bool syslog_manager::connect_syslog()
{
    if (!device)
    {
        std::cerr << "Error: Device not connected." << std::endl;
        return false;
    }

    if (syslog_connected)
    {
        std::cout << "Syslog already connected." << std::endl;
        return true;
    }

    if (syslog_relay_client_start_service(device, &syslog_client, "security-tool")
        != SYSLOG_RELAY_E_SUCCESS)
    {
        std::cerr << "Error: Failed to start syslog relay service." << std::endl;
        return false;
    }

    syslog_connected = true;
    std::cout << "Syslog connection established." << std::endl;
    return true;
}

/*****************************************************************************/
/* Function Name: disconnect                                                 */
/*                                                                           */
/* Description: Disconnects from syslog service, stopping capture if active  */
/*                                                                           */
/* Date            Engineer              Comments                            */
/* ------------------------------------------------------------------------- */
/* 2026-02-12      S. Amalfitano         Initial implementation              */
/*****************************************************************************/
void syslog_manager::disconnect()
{
    if (is_capturing)
    {
        stop_capture();
    }

    if (syslog_connected && syslog_client)
    {
        syslog_relay_client_free(syslog_client);
        syslog_client = nullptr;
        syslog_connected = false;
    }
}

/*****************************************************************************/
/* Function Name: syslog_callback_wrapper                                    */
/*                                                                           */
/* Description: Static callback function that receives characters from the   */
/*              syslog stream and builds complete lines                      */
/*                                                                           */
/* Date            Engineer              Comments                            */
/* ------------------------------------------------------------------------- */
/* 2026-02-12      S. Amalfitano         Initial implementation              */
/*****************************************************************************/
void syslog_manager::syslog_callback_wrapper(char c, void* user_data)
{
    syslog_manager* manager = static_cast<syslog_manager*>(user_data);

    if (c == '\n' || c == '\r')
    {
        if (!manager->current_line.empty() && manager->line_callback)
        {
            manager->line_callback(manager->current_line);
            manager->current_line.clear();
        }
    }
    else
    {
        manager->current_line += c;
    }
}

/*****************************************************************************/
/* Function Name: start_capture                                              */
/*                                                                           */
/* Description: Starts capturing syslog output and calls the provided        */
/*              callback function for each complete log line                 */
/*                                                                           */
/* Date            Engineer              Comments                            */
/* ------------------------------------------------------------------------- */
/* 2026-02-12      S. Amalfitano         Initial implementation              */
/*****************************************************************************/
bool syslog_manager::start_capture(std::function<void(const std::string&)> callback)
{
    if (!syslog_connected)
    {
        std::cerr << "Error: Syslog not connected. Call connect_syslog() first." << std::endl;
        return false;
    }

    if (is_capturing)
    {
        std::cout << "Syslog capture already active." << std::endl;
        return true;
    }

    line_callback = callback;
    current_line.clear();

    if (syslog_relay_start_capture(syslog_client, syslog_callback_wrapper, this)
        != SYSLOG_RELAY_E_SUCCESS)
    {
        std::cerr << "Error: Failed to start syslog capture." << std::endl;
        return false;
    }

    is_capturing = true;
    std::cout << "Syslog capture started." << std::endl;
    return true;
}

/*****************************************************************************/
/* Function Name: stop_capture                                               */
/*                                                                           */
/* Description: Stops capturing syslog output                                */
/*                                                                           */
/* Date            Engineer              Comments                            */
/* ------------------------------------------------------------------------- */
/* 2026-02-12      S. Amalfitano         Initial implementation              */
/*****************************************************************************/
bool syslog_manager::stop_capture()
{
    if (!is_capturing)
    {
        std::cout << "Syslog capture is not active." << std::endl;
        return true;
    }

    if (syslog_relay_stop_capture(syslog_client) != SYSLOG_RELAY_E_SUCCESS)
    {
        std::cerr << "Error: Failed to stop syslog capture." << std::endl;
        return false;
    }

    is_capturing = false;
    std::cout << "Syslog capture stopped." << std::endl;
    return true;
}

/*****************************************************************************/
/* Function Name: is_connected                                               */
/*                                                                           */
/* Description: Returns true if syslog connection is established             */
/*                                                                           */
/* Date            Engineer              Comments                            */
/* ------------------------------------------------------------------------- */
/* 2026-02-12      S. Amalfitano         Initial implementation              */
/*****************************************************************************/
bool syslog_manager::is_connected() const
{
    return syslog_connected;
}

/*****************************************************************************/
/* Function Name: is_capturing_logs                                          */
/*                                                                           */
/* Description: Returns true if currently capturing syslog output            */
/*                                                                           */
/* Date            Engineer              Comments                            */
/* ------------------------------------------------------------------------- */
/* 2026-02-12      S. Amalfitano         Initial implementation              */
/*****************************************************************************/
bool syslog_manager::is_capturing_logs() const
{
    return is_capturing;
}
