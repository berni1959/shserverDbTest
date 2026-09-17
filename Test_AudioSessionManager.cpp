#include "Tests.h"
#include "AudioSessionManager.h"
#include "Logger.h"
#include <iostream>
#include <thread>
#include <chrono>

void Test_AudioSessionManager()
{
    AudioSessionManager audio;

    std::cout << "Starte Audio-Aufnahme (10 Sekunden) - sprechen Sie vor die DoorBird-Kamera!\n";

    bool started = audio.Start("192.168.0.63", "ghsjms0008", "Ga1012Be1006", "D:\\DoorbirdRecordings", "test");

    if (!started)
    {
        std::cout << "Start fehlgeschlagen\n";
        return;
    }

    std::this_thread::sleep_for(std::chrono::seconds(10));

    std::cout << "Stoppe...\n";
    audio.Stop();

    std::cout << "Fertig. Pruefen Sie D:\\DoorbirdRecordings\\doorbird\\...\n";
}