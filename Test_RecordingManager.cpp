#include "Tests.h"
#include "RecordingManager.h"
#include "Logger.h"
#include <iostream>
#include <thread>
#include <chrono>

void Test_RecordingManager()
{
    RecordingManager rec;

    std::string streamUrl = "http://ghsjms0008:Ga1012Be1006@192.168.0.63/bha-api/video.cgi";

    std::cout << "Starte Testaufnahme (10 Sekunden)...\n";

    bool started = rec.Start("D:\\DoorbirdRecordings", streamUrl, "test", 10);

    if (!started)
    {
        std::cout << "Start fehlgeschlagen\n";
        return;
    }

    std::cout << "Laeuft... (ffmpeg begrenzt automatisch auf 10 Sekunden)\n";

    // Warten, bis ffmpeg von selbst fertig ist (wegen -t 10)
    while (rec.IsRecording())
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }

    std::cout << "Aufnahme beendet. Pruefen Sie C:\\DoorbirdRecordings\\doorbird\\...\n";
}