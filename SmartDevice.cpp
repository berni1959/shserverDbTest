#include "SmartDevice.h"
#include <charconv>

std::optional<double> SmartDevice::AsNumber() const
{
    if (current_value.empty())
    {
        return std::nullopt;
    }

    double result = 0.0;
    auto begin = current_value.data();
    auto end = current_value.data() + current_value.size();
    auto [ptr, ec] = std::from_chars(begin, end, result);

    if (ec != std::errc())
    {
        return std::nullopt;
    }
    return result;
}