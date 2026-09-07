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
	int InsertDevice(const SmartDevice& device);
	void UpdateDevice(const SmartDevice& device);
	void UpdateDeviceState(int deviceId, const std::string& currentValue);
	void DeleteDevice(int deviceId);
	int InsertGroupAddress(const DeviceGroupAddress& ga);
	void UpdateGroupAdress(const DeviceGroupAddress& ga);
	void DeleteGroupAddress(int id);


private:
    DbConnection& m_connection;
    SmartDevice MapDeviceRow(MYSQL_ROW row);
	DeviceGroupAddress MapGroupAddressRow(MYSQL_ROW row);
	std::string SqlValue(const std::string& value);	

    std::vector<SmartDevice> QueryDevices(const std::string& whereClause);

    std::string Escape(const std::string& value);
};