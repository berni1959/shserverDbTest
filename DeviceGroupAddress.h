#pragma once
#include <string>

// Spiegelt eine Zeile der Tabelle device_group_addresses.
// Zusätzliche Gruppenadressen zu einem Gerät aus smart_devices,
// über device_id verknüpft (1 Gerät -> viele GAs).
struct DeviceGroupAddress
{
    int id = 0;
    int device_id = 0;
    std::string function_name;
    std::string ga;
    std::string dpt;
    std::string notes;
};