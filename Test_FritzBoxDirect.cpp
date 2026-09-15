#include "Tests.h"
#include "FritzClient.h"
#include "Logger.h"
#include <iostream>

void Test_FritzBoxDirect()
{
    FritzClient fritz("192.168.0.1", "Bernd", "Ga1012#Be1006#");

    std::cout << "Logge ein...\n";
    if (!fritz.Login())
    {
        std::cout << "Login fehlgeschlagen\n";
        return;
    }

    std::cout << "Login erfolgreich! Hole Geraeteliste...\n";
    auto devices = fritz.GetDeviceList();

    std::cout << "Anzahl Geraete: " << devices.size() << "\n";

    for (const auto& dev : devices)
    {
        std::cout << dev.name << " (AIN " << dev.ain << ")";
        if (dev.temperature) std::cout << " Temp=" << *dev.temperature;
        if (dev.battery) std::cout << " Batterie=" << *dev.battery << "%";
        if (dev.hkrIst) std::cout << " Ist=" << *dev.hkrIst;
        if (dev.hkrSoll) std::cout << " Soll=" << *dev.hkrSoll;
        if (dev.state) std::cout << " Zustand=" << *dev.state;
        std::cout << "\n";
    }
}