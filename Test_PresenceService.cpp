#include "Tests.h"
#include "AlarmArmService.h"
#include "PresenceService.h"
#include "TelegramNotifier.h"
#include "PingClient.h"
#include "Logger.h"
#include <iostream>
#include <thread>
#include <chrono>
#include <map>

void Test_PresenceService()
{
    AlarmArmService alarmService("1234");

    std::map<std::string, std::string> chatIds = {
        { "Bernd", "44909982" },
        { "Frau", "8862924257" }
    };
    TelegramNotifier telegram("8380331116:AAHRjMzIRQMUL2slKRMJbHUpBNCVqBgKsAE", chatIds);

    std::vector<std::pair<std::string, std::string>> phoneIps = {
        { "Gaby", "192.168.0.184" },
        { "Note20", "192.168.0.180" },
        { "Fold5", "192.168.0.187" }
    };

    PresenceService presence(alarmService, telegram, phoneIps, 5);

    std::cout << "Laufender Test - Strg+C zum Beenden\n\n";

    while (true)
    {
        std::cout << "--- Pruefung " << std::time(nullptr) << " ---\n";

        for (const auto& [name, ip] : phoneIps)
        {
            bool reachable = PingClient::IsReachable(ip);
            std::cout << "  " << name << " (" << ip << "): " << (reachable ? "ONLINE" : "offline") << "\n";
        }

        presence.CheckOnce();

        std::cout << "Alarm-Status: " << (alarmService.IsArmed() ? "SCHARF" : "unscharf") << "\n\n";

        std::this_thread::sleep_for(std::chrono::seconds(30));
    }
}