#include "Tests.h"
#include "SmartHomeMqttClient.h"
#include "XSenseManager.h"
#include "Logger.h"
#include <iostream>
#include <thread>
#include <chrono>

void Test_XSense()
{
    XSenseManager xsense;

    xsense.onUpdated = [](const std::string& deviceId) {
        std::cout << "X-Sense aktualisiert: " << deviceId << "\n";
        };
    xsense.onSmokeAlarm = [](const std::string& deviceId) {
        std::cout << "!!! RAUCHALARM: " << deviceId << " !!!\n";
        };

    SmartHomeMqttClient client("tcp://192.168.0.9:1883", "shserver-cpp-xsense-test");

    client.SetOnMessage([&](const std::string& topic, const std::string& payload) {
        xsense.HandleMessage(topic, payload);
        });

    if (!client.Connect())
    {
        std::cout << "Verbindung fehlgeschlagen\n";
        return;
    }

    client.Subscribe("homeassistant/binary_sensor/#");

    std::cout << "Warte auf X-Sense-Status (30 Sekunden)...\n";
    std::this_thread::sleep_for(std::chrono::seconds(30));

    client.Disconnect();
}