#pragma once
#include <string>
#include <optional>
#include <vector>
#include "DeviceGroupAddress.h"

struct SmartDevice
{
    int id = 0;

    std::string ga;
    std::string ga_up;
    std::string ga_down;
    std::string ga_stop;
    std::string ga_position;
    std::string ga_status;
    std::string name;
    std::string device_id;
    std::string device_type;
    std::string mqtt_topic;
    std::string http_ip;
    std::string protocol;
    std::string category;
    bool enabled = true;
    std::string icon;
    std::string dpt;
    std::string unit;
    std::string room;
    std::string current_value;
    std::string last_update;
    bool state_bool = false;

    std::string alias;
    std::optional<double> AsNumber() const;
    std::vector<DeviceGroupAddress> extraGroupAddresses;
    bool IsOn() const { return state_bool || current_value == "1"; }



};