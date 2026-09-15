#include "Tests.h"
#include "SolarmanClient.h"
#include "Logger.h"
#include <iostream>

void Test_Solarman()
{
    SolarmanClient solarman("192.168.0.146", 3834384963);

    std::cout << "Lese Register...\n";
    auto regs = solarman.ReadInputRegisters(0x00, 0x3C);

    if (regs.empty())
    {
        std::cout << "Lesen fehlgeschlagen\n";
        return;
    }

    std::cout << "Anzahl Register: " << regs.size() << "\n";

    double pv1Voltage = regs[0x00] * 0.1;
    double pv2Voltage = regs[0x01] * 0.1;
    double acCurrent = regs[0x09] * 0.01;
    double acVoltage = regs[0x0B] * 0.1;
    double acFrequency = regs[0x0D] * 0.01;
    double acPowerW = regs[0x0F] * 0.1;
    double totalEnergyKwh = regs[0x14] * 0.01;

    std::cout << "PV1-Spannung: " << pv1Voltage << "V\n";
    std::cout << "PV2-Spannung: " << pv2Voltage << "V\n";
    std::cout << "AC-Strom: " << acCurrent << "A\n";
    std::cout << "AC-Spannung: " << acVoltage << "V\n";
    std::cout << "AC-Frequenz: " << acFrequency << "Hz\n";
    std::cout << "AC-Leistung: " << acPowerW << "W\n";
    std::cout << "Gesamtenergie: " << totalEnergyKwh << "kWh\n";
}