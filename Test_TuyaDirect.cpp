#include "Tests.h"
#include "TuyaClient.h"
#include "Logger.h"
#include <iostream>

void Test_TuyaDirect()
{
    TuyaClient tuya("192.168.0.136", "8166258640915177d7cf", "r)oScy[I.zL4c/ge");

    std::cout << "Frage Status ab...\n";
    std::string status = tuya.GetStatus();

    if (status.empty())
    {
        std::cout << "Status-Abfrage fehlgeschlagen\n";
        return;
    }

    std::cout << "Status: " << status << "\n";
}