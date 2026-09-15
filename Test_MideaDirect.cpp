#include "Tests.h"
#include "MideaClient.h"
#include "Logger.h"
#include <iostream>

void Test_MideaDirect()
{
    MideaClient midea("192.168.0.127", 18691697793754ULL);

    std::cout << "Frage Status ab...\n";
    auto state = midea.GetState();

    if (!state)
    {
        std::cout << "Status-Abfrage fehlgeschlagen\n";
        return;
    }

    std::cout << "Power=" << state->powerOn
        << " Soll=" << state->targetTemperature
        << " Modus=" << state->operationalMode
        << " Luefter=" << state->fanSpeed
        << " Swing=" << state->swingMode
        << " Turbo=" << state->turbo
        << " Eco=" << state->eco
        << " DisplayOn=" << state->displayOn;

    if (state->indoorTemperature) std::cout << " Innen=" << *state->indoorTemperature;
    if (state->outdoorTemperature) std::cout << " Aussen=" << *state->outdoorTemperature;

    std::cout << "\n";
}