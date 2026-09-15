#include "Tests.h"
#include "DoorbirdClient.h"
#include "DoorbirdWebhookServer.h"
#include "TelegramNotifier.h"
#include "Logger.h"
#include <iostream>
#include <thread>
#include <chrono>
#include <map>

void Test_DoorBird()
{
    DoorbirdClient doorbird("192.168.0.63", "IhrBenutzername", "IhrPasswort");

    std::map<std::string, std::string> chatIds = {
        { "Bernd", "44909982" },
        { "Frau", "8862924257" }
    };
    TelegramNotifier telegram("8380331116:AAHRjMzIRQMUL2slKRMJbHUpBNCVqBgKsAE", chatIds);

    DoorbirdWebhookServer server(5070);

    server.onDoorbell = [&]() {
        std::cout << "Klingel! Hole Snapshot und sende an Telegram...\n";
        auto snapshot = doorbird.GetSnapshot();
        if (!snapshot.empty())
        {
            telegram.SendPhoto(snapshot, "Es hat geklingelt!", "Alle");
        }
        };

    server.onMotion = [&]() {
        std::cout << "Bewegung erkannt!\n";
        auto snapshot = doorbird.GetSnapshot();
        if (!snapshot.empty())
        {
            telegram.SendPhoto(snapshot, "Bewegung an der Tuer erkannt", "Alle");
        }
        };

    server.Start();

    std::cout << "Server laeuft auf Port 5070 (60 Sekunden)...\n";
    std::cout << "Test: http://localhost:5070/api/event/doorbell1\n";

    std::this_thread::sleep_for(std::chrono::seconds(60));

    server.Stop();
}