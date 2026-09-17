#include "DoorbirdWebhookServer.h"
#include "DoorbirdClient.h"
#include "TelegramNotifier.h"
#include "DbConnection.h"
#include "ScheduleRunner.h"
#include "KnxTunnelingProtocol.h"
#include "KnxSwitchService.h"
#include "AlarmArmService.h"
#include "PresenceService.h"
#include "Logger.h"

#include <windows.h>
#include <iostream>
#include <thread>
#include <chrono>
#include <atomic>
#include <map>

std::atomic<bool> g_running{ true };

void ScheduleLoop(ScheduleRunner& runner)
{
    while (g_running)
    {
        try
        {
            runner.CheckSchedulesOnce();
        }
        catch (const std::exception& ex)
        {
            Logger::Error(std::string("ScheduleLoop: ") + ex.what());
        }
        std::this_thread::sleep_for(std::chrono::seconds(30));
    }
}

void PresenceLoop(PresenceService& presence)
{
    while (g_running)
    {
        try
        {
            presence.CheckOnce();
        }
        catch (const std::exception& ex)
        {
            Logger::Error(std::string("PresenceLoop: ") + ex.what());
        }
        std::this_thread::sleep_for(std::chrono::seconds(30));
    }
}

int main()
{
    SetConsoleOutputCP(CP_UTF8);
    Logger::Init("C:\\dev\\logs\\smarthome.log");
    Logger::SetConsoleOutput(true);

    Logger::Info("=== SmartHome Backend startet ===");

    // --- Telegram ---
    std::map<std::string, std::string> chatIds = {
        { "Bernd", "44909982" },
        { "Frau", "8862924257" }
    };
    TelegramNotifier telegram("8380331116:AAHRjMzIRQMUL2slKRMJbHUpBNCVqBgKsAE", chatIds);

    // --- DoorBird Webhook (Klingel/Bewegung) ---
    DoorbirdClient doorbird("192.168.0.63", "ghsjms0008", "Ga1012Be1006");
    DoorbirdWebhookServer webhookServer(5070);

    webhookServer.onDoorbell = [&]() {
        auto snapshot = doorbird.GetSnapshot();
        if (!snapshot.empty())
        {
            telegram.SendPhoto(snapshot, "Es hat geklingelt!", "Alle");
        }
        };
    webhookServer.onMotion = [&]() {
        auto snapshot = doorbird.GetSnapshot();
        if (!snapshot.empty())
        {
            telegram.SendPhoto(snapshot, "Bewegung an der Tuer erkannt", "Alle");
        }
        };
    webhookServer.Start();

    // --- KNX ---
    KnxTunnelingProtocol knxProtocol("192.168.0.66");
    if (!knxProtocol.Connect())
    {
        Logger::Error("KNX-Verbindung fehlgeschlagen - Zeitschaltuhr wird nicht funktionieren");
    }
    KnxSwitchService knx(knxProtocol);

    // --- Datenbank + Zeitschaltuhr ---
    DbConnection dbConn("192.168.0.28", "sa", "spEDV2020#", "Smarthome", 3306);
    ScheduleRunner scheduleRunner(dbConn, knx, telegram, 49.317, 8.433);
    std::thread scheduleThread(ScheduleLoop, std::ref(scheduleRunner));

    // --- Presence / Alarm ---
    AlarmArmService alarmService("1234");
    std::vector<std::pair<std::string, std::string>> phoneIps = {
        { "Gaby", "192.168.0.184" },
        { "Note20", "192.168.0.180" },
        { "Fold5", "192.168.0.187" }
    };
    PresenceService presenceService(alarmService, telegram, phoneIps, 5);
    std::thread presenceThread(PresenceLoop, std::ref(presenceService));

    Logger::Info("=== Alle Dienste gestartet - druecken Sie Enter zum Beenden ===");
    std::cin.get();

    Logger::Info("=== Beende... ===");
    g_running = false;

    scheduleThread.join();
    presenceThread.join();
    webhookServer.Stop();
    knxProtocol.Disconnect();

    Logger::Info("=== SmartHome Backend beendet ===");
    return 0;
}