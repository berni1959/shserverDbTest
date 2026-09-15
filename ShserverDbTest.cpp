#include "Tests.h"
#include "Logger.h"
#include <windows.h>
#include <iostream>
#include <string>

int main()
{
    SetConsoleOutputCP(CP_UTF8);
    Logger::Init("C:\\dev\\logs\\smarthome.log");
    Logger::SetConsoleOutput(true);

    std::cout << "Welcher Test soll laufen?\n";
    std::cout << " 1 - Solar (Huawei + Solarman + KNX)\n";
    std::cout << " 2 - Shelly\n";
    std::cout << " 3 - FritzBox\n";
    std::cout << " 4 - Tuya\n";
    std::cout << " 5 - Midea\n";
    std::cout << " 6 - DoorBird\n";
    std::cout << " 7 - X-Sense\n";
    std::cout << " 8 - Dyson\n";
    std::cout << " 9 - Solarman (nur Auslesen)\n";
    std::cout << "10 - Tuya (direkt, ohne Pi)\n";
    std::cout << "11 - Midea (direkt, ohne Pi)\n";
    std::cout << "Auswahl: ";

    int choice;
    std::cin >> choice;

    switch (choice)
    {
    case 1: Test_Solar(); break;
    case 2: Test_Shelly(); break;
    case 3: Test_FritzBox(); break;
    case 4: Test_Tuya(); break;
    case 5: Test_Midea(); break;
    case 6: Test_DoorBird(); break;
    case 7: Test_XSense(); break;
    case 8: Test_Dyson(); break;
    case 9: Test_Solarman(); break;
    case 10: Test_TuyaDirect(); break;
    case 11: Test_MideaDirect(); break;
    default: std::cout << "Ungueltige Auswahl\n";
    }

    std::cout << "\nFertig. Taste druecken zum Beenden...\n";
    std::cin.ignore();
    std::cin.get();

    return 0;
}