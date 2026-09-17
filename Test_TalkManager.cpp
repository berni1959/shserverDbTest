#include "Tests.h"
#include "TalkManager.h"
#include "Logger.h"
#include <iostream>
#include <thread>
#include <chrono>

void Test_TalkManager()
{
    TalkManager talk;

    std::cout << "Starte Sprechverbindung...\n";
    bool started = talk.Start("192.168.0.63", "ghsjms0008", "Ga1012Be1006");

    if (!started)
    {
        std::cout << "Start fehlgeschlagen\n";
        return;
    }

    std::cout << "Verbindung laeuft 5 Sekunden (keine echten Audiodaten, nur Verbindungstest)...\n";
    std::this_thread::sleep_for(std::chrono::seconds(5));

    std::cout << "Stoppe...\n";
    talk.Stop();

    std::cout << "Fertig.\n";
}