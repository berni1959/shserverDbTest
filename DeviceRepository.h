#pragma once
#include "DbConnection.h"
#include "SmartDevice.h"
#include "DeviceGroupAddress.h"
#include <vector>
#include <optional>
#include <string>
#include <map>

class DeviceRepository
{
public:
    explicit DeviceRepository(DbConnection& connection);

    std::vector<SmartDevice> GetAll();

    std::optional<SmartDevice> GetByGA(const std::string& ga);

    std::vector<SmartDevice> GetByProtocol(const std::string& protocol);

private:
    DbConnection& m_connection;
    SmartDevice MapDeviceRow(MYSQL_ROW row);
	DeviceGroupAddress MapGroupAddressRow(MYSQL_ROW row);


    std::vector<SmartDevice> QueryDevices(const std::string& whereClause);

    std::string Escape(const std::string& value);
};