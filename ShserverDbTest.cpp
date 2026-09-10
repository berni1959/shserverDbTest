#include "SmartHomeMqttClient.h"
#include "SolarState.h"
#include "SolarRepository.h"
#include "DbConnection.h"
#include "KnxRoutingListener.h"
#include "KnxTelegram.h"
#include "KnxDptDecoder.h"
#include "Logger.h"
#include <windows.h>
#include <iostream>
#include <thread>
#include <atomic>
#include <mutex>
#include <chrono>
#include "SolarmanState.h"

std::mutex g_knxMutex;
double g_lux = 0.0;
double g_sunAzimuth = 0.0;
double g_sunElevation = 0.0;
double g_temperature = 0.0;
bool g_luxReceived = false;
bool g_sunAzimuthReceived = false;
bool g_sunElevationReceived = false;
bool g_temperatureReceived = false;


void KnxBackgroundLoop(std::atomic<bool>& running)
{
    KnxRoutingListener listener;
    listener.Start("224.0.23.12", 3671);

    while (running)
    {
        auto raw = listener.ReceiveOne();
        if (raw.empty())
        {
            continue;
        }

        auto telegram = ParseCemiFrame(raw);
        if (!telegram || telegram->isGroupRead)
        {
            continue;
        }

        if (telegram->groupAddress == "3/3/0")
        {
            double value = std::stod(KnxDptDecoder::Decode("DPST-9-4", telegram->data));
            std::lock_guard<std::mutex> lock(g_knxMutex);
            g_lux = value;
			g_luxReceived = true;   
        }
        else if (telegram->groupAddress == "3/7/0")
        {
            double value = std::stod(KnxDptDecoder::Decode("DPST-14-7", telegram->data));
            std::lock_guard<std::mutex> lock(g_knxMutex);
            g_sunAzimuth = value;
			g_sunAzimuthReceived = true;
        }
        else if (telegram->groupAddress == "3/7/10")
        {
            double value = std::stod(KnxDptDecoder::Decode("DPST-14-7", telegram->data));
            std::lock_guard<std::mutex> lock(g_knxMutex);
            g_sunElevation = value;
			g_sunElevationReceived = true;
        }
        else if (telegram->groupAddress == "3/0/0")
        {
            double value = std::stod(KnxDptDecoder::Decode("DPST-9-1", telegram->data));
            std::lock_guard<std::mutex> lock(g_knxMutex);
            g_temperature = value;
			g_temperatureReceived = true;
        }
    }

    listener.Stop();
}

int main()
{
    SetConsoleOutputCP(CP_UTF8);
    Logger::Init("C:\\dev\\logs\\smarthome.log");
    Logger::SetConsoleOutput(true);

    std::atomic<bool> running{ true };
    std::thread knxThread(KnxBackgroundLoop, std::ref(running));

    SolarState solar;
    SolarmanState solarman;
    auto lastSaved = std::chrono::steady_clock::time_point{};

    SmartHomeMqttClient client("tcp://192.168.0.9:1883", "shserver-cpp-solar-test");

    client.SetOnMessage([&](const std::string& topic, const std::string& payload) {
        if (topic == "home/solarman/tmL800m")
        {
            UpdateSolarmanState(solarman, payload);
            std::cout << "Solarman aktualisiert: " << solarman.acPowerW << "W\n";
            return;
        }
        if (topic.rfind("home/solar/", 0) != 0)
        {
            return;
        }

        if (!UpdateSolarState(solar, payload))
        {
            return;
        }


        bool knxReady;
        double lux, sunAzimuth, sunElevation, temperature;
        {
            std::lock_guard<std::mutex> lock(g_knxMutex);
            lux = g_lux;
            sunAzimuth = g_sunAzimuth;
            sunElevation = g_sunElevation;
            temperature = g_temperature;
            knxReady = g_luxReceived && g_sunAzimuthReceived && g_sunElevationReceived && g_temperatureReceived;
        }

        if (!knxReady)
        {
            Logger::Info("Warte noch auf vollstaendige KNX-Werte, Speichern uebersprungen");
            return;
        }


        if (topic.rfind("home/solar/", 0) != 0)
        {
            return;
        }

        if (!UpdateSolarState(solar, payload))
        {
            return;
        }



        std::cout << "Solar: " << solar.currentPowerKw << "kW, Lux=" << lux
            << ", Sonnenstand=" << sunAzimuth << "/" << sunElevation << ", Temperatur=" << temperature << "\n";
        auto now = std::chrono::steady_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::minutes>(now - lastSaved);

        if (elapsed.count() >= 15)
        {
            try
            {
                DbConnection dbConn("192.168.0.28", "sa", "spEDV2020#", "Smarthome", 3306);
                SolarRepository repo(dbConn);
                repo.InsertReading(solar, lux, sunAzimuth, sunElevation, temperature,
                    solarman.acPowerW, solarman.totalEnergyKwh, solarman.acVoltage, solarman.acCurrent);
                lastSaved = now;
                Logger::Info("Solar-Snapshot inkl. KNX-Werte gespeichert");
            }
            catch (const DbException& ex)
            {
                Logger::Error(std::string("Speichern fehlgeschlagen: ") + ex.what());
            }
        }
        });

    if (!client.Connect())
    {
        std::cerr << "Verbindung fehlgeschlagen\n";
        running = false;
        knxThread.join();
        return 1;
    }

    client.Subscribe("home/solar/#");
    client.Subscribe("home/solarman/tmL800m");
    std::cout << "Laeuft 300 Sekunden...\n";
    std::this_thread::sleep_for(std::chrono::seconds(300));

    client.Disconnect();
    running = false;
    knxThread.join();

    return 0;
}