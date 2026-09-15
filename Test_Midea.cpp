#include "Tests.h"
#include "SmartHomeMqttClient.h"
#include "MideaState.h"
#include "Logger.h"
#include <iostream>
#include <thread>
#include <chrono>

void Test_Midea()
{
    MideaState midea;

    SmartHomeMqttClient client("tcp://192.168.0.9:1883", "shserver-cpp-midea-test");

    client.SetOnMessage([&](const std::string& topic, const std::string& payload) {
        if (topic != "home/midea/ac")
        {
            return;
        }

        if (!UpdateMideaState(midea, payload))
        {
            return;
        }

        std::cout << "Midea: Power=" << midea.power
            << " Mode=" << midea.mode
            << " Soll=" << midea.targetTemperature
            << " Ist=" << midea.indoorTemperature
            << " Swing=" << midea.swingMode << "\n";
        });

    if (!client.Connect())
    {
        std::cout << "Verbindung fehlgeschlagen\n";
        return;
    }

    client.Subscribe("home/midea/ac");

    std::cout << "Warte auf Midea-Status (15 Sekunden)...\n";
    std::this_thread::sleep_for(std::chrono::seconds(15));

    client.Disconnect();
}