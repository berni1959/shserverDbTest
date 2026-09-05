#include "DbConnection.h"
#include "DeviceRepository.h"
#include <iostream>
#include <windows.h>
int main()
{
    SetConsoleOutputCP(CP_UTF8);
    try
    {
        DbConnection connection("192.168.0.28", "sa", "spEDV2020#", "Smarthome", 3306);
        std::cout << "Verbindung erfolgreich hergestellt!\n\n";

        DeviceRepository deviceRepo(connection);

        auto all = deviceRepo.GetAll();
        std::cout << "Anzahl Geraete gesamt: " << all.size() << "\n\n";

        // Geraete mit zusaetzlichen GAs anzeigen
        for (const auto& device : all)
        {
            if (!device.extraGroupAddresses.empty())
            {
                std::cout << device.name << " (ID " << device.id << ") hat "
                    << device.extraGroupAddresses.size() << " zusaetzliche GA(s):\n";
                for (const auto& ga : device.extraGroupAddresses)
                {
                    std::cout << "  - " << ga.function_name << ": " << ga.ga << "\n";
                }
            }
        }
    }
    catch (const DbException& ex)
    {
        std::cerr << "DB-Fehler: " << ex.what() << "\n";
        return 1;
    }

    return 0;
}