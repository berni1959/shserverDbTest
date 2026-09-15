#include "Tests.h"
#include "DysonBridge.h"
#include "Logger.h"
#include <iostream>
#include <thread>
#include <chrono>

void Test_Dyson()
{
    DysonBridge bridge(
        "192.168.0.103", "A1B-EU-MBA0099A",
        "chsj02b7S0uX/N3KQ5WMMWcVynO4+/bUrbcfaQfIWFCEfj6LeTDdZOEns7oV2fbex1YlGQx9hLmSJ3TqtKGwLA==",
        "192.168.0.97"
    );

    if (!bridge.Start())
    {
        std::cout << "Start fehlgeschlagen\n";
        return;
    }

    std::cout << "DysonBridge laeuft (60 Sekunden)...\n";
    std::this_thread::sleep_for(std::chrono::seconds(60));

    bridge.Stop();
}