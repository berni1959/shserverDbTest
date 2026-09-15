#include "Tests.h"
#include "SunCalc.h"
#include <iostream>
#include <ctime>

void Test_SunCalc()
{
    time_t now = time(nullptr);
    std::tm today;
    localtime_s(&today, &now);

    double latitude = 49.317;
    double longitude = 8.433;

    auto sunrise = SunCalc::GetSunrise(today, latitude, longitude);
    auto sunset = SunCalc::GetSunset(today, latitude, longitude);

    char buf[64];
    std::strftime(buf, sizeof(buf), "%H:%M:%S", &sunrise);
    std::cout << "Sonnenaufgang: " << buf << "\n";

    std::strftime(buf, sizeof(buf), "%H:%M:%S", &sunset);
    std::cout << "Sonnenuntergang: " << buf << "\n";
}