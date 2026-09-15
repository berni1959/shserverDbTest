#include "Tests.h"
#include "DbConnection.h"
#include "ScheduleRunner.h"
#include "KnxTunnelingProtocol.h"
#include "KnxSwitchService.h"
#include "TelegramNotifier.h"
#include "Logger.h"
#include <iostream>
#include <thread>
#include <chrono>
#include <map>

void Test_ScheduleRunner()
{
    try
    {
        DbConnection dbConn("192.168.0.28", "sa", "spEDV2020#", "Smarthome", 3306);

        KnxTunnelingProtocol knxProtocol("192.168.0.66");
        if (!knxProtocol.Connect())
        {
            std::cout << "KNX-Verbindung fehlgeschlagen\n";
            return;
        }

        KnxSwitchService knx(knxProtocol);

        std::map<std::string, std::string> chatIds = {
            { "Bernd", "44909982" },
            { "Frau", "8862924257" }
        };
        TelegramNotifier telegram("8380331116:AAHRjMzIRQMUL2slKRMJbHUpBNCVqBgKsAE", chatIds);

        ScheduleRunner runner(dbConn, knx, telegram, 49.317, 8.433);

        std::cout << "Pruefe Zeitplaene (einmalig)...\n";
        runner.CheckSchedulesOnce();
        std::cout << "Fertig.\n";

        knxProtocol.Disconnect();
    }
    catch (const DbException& ex)
    {
        std::cout << "Datenbankfehler: " << ex.what() << "\n";
    }
}