#include "Tests.h"
#include "SmartHomeMqttClient.h"
#include "TuyaAcState.h"
#include "TuyaMqttCommands.h"
#include "Logger.h"
#include <iostream>
#include <thread>
#include <chrono>

void Test_Tuya()
{
    TuyaAcState tuya;

    SmartHomeMqttClient client("tcp://192.168.0.9:1883", "shserver-cpp-tuya-test");

    client.SetOnMessage([&](const std::string& topic, const std::string& payload) {
        if (topic != "shserver/tuya/wohnzimmer_ac/state")
        {
            return;
        }

        if (!UpdateTuyaAcState(tuya, payload))
        {
            return;
        }

        std::cout << "Tuya: Power=" << tuya.power;
        if (tuya.currentTemp) std::cout << " Ist=" << *tuya.currentTemp;
        if (tuya.targetTemp) std::cout << " Soll=" << *tuya.targetTemp;
        if (tuya.mode) std::cout << " Modus=" << *tuya.mode;
        if (tuya.fan) std::cout << " Luefter=" << *tuya.fan;
        if (tuya.swingHorizontal) std::cout << " SwingH=" << *tuya.swingHorizontal;
        if (tuya.swingVertical) std::cout << " SwingV=" << *tuya.swingVertical;
        std::cout << "\n";
        });

    if (!client.Connect())
    {
        std::cout << "Verbindung fehlgeschlagen\n";
        return;
    }

    client.Subscribe("shserver/tuya/wohnzimmer_ac/state");

    std::cout << "Warte auf Tuya-Status (15 Sekunden)...\n";
    std::this_thread::sleep_for(std::chrono::seconds(15));

    client.Disconnect();
}