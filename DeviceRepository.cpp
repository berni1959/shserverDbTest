#include "DeviceRepository.h"
#include <stdexcept>
#include "DeviceGroupAddress.h"

namespace
{
    const char* kDeviceColumns =
        "d.id, d.ga, d.ga_up, d.ga_down, d.ga_stop, d.ga_position, d.ga_status, "
        "d.name, d.device_id, d.device_type, "
        "d.mqtt_topic, d.http_ip, d.protocol, d.category, "
        "d.enabled, d.icon, d.dpt, d.unit, d.room, "
        "d.current_value, d.last_update, d.state_bool, d.alias";

    const char* kGroupColumns =
        "g.id, g.device_id, g.function_name, g.ga, g.dpt, g.notes";
   
    std::string ColumnAsString(MYSQL_ROW row, int index)
    {
        return row[index] ? std::string(row[index]) : std::string();
    }

    bool ColumnAsBool(MYSQL_ROW row, int index)
    {
        return row[index] && std::string(row[index]) == "1";
    }
}
    DeviceRepository::DeviceRepository(DbConnection& connection) : m_connection(connection)
    {
    }

    std::string DeviceRepository::Escape(const std::string& value)
    {
        std::string escaped(value.size() * 2 + 1, '\0');
        unsigned long len = mysql_real_escape_string(
            m_connection.Raw(), escaped.data(), value.c_str(), (unsigned long)value.size());
        escaped.resize(len);
        return escaped;
    }

    std::string DeviceRepository::SqlValue(const std::string& value) //macht leerstring wenn feld leer ist
    {
        if (value.empty())
        {
            return "NULL";
        }
        else
        {
            return "'" + Escape(value) + "'";
        }
	}



    SmartDevice DeviceRepository::MapDeviceRow(MYSQL_ROW row)
    {
        SmartDevice device;
        int i = 0;

        device.id = row[i] ? std::atoi(row[i]) : 0; i++;
        device.ga = ColumnAsString(row, i++);
        device.ga_up = ColumnAsString(row, i++);
        device.ga_down = ColumnAsString(row, i++);
        device.ga_stop = ColumnAsString(row, i++);
        device.ga_position = ColumnAsString(row, i++);
        device.ga_status = ColumnAsString(row, i++);

        device.name = ColumnAsString(row, i++);
        device.device_id = ColumnAsString(row, i++);
        device.device_type = ColumnAsString(row, i++);

        device.mqtt_topic = ColumnAsString(row, i++);
        device.http_ip = ColumnAsString(row, i++);
        device.protocol = ColumnAsString(row, i++);
        device.category = ColumnAsString(row, i++);

        device.enabled = ColumnAsBool(row, i++);
        device.icon = ColumnAsString(row, i++);
        device.dpt = ColumnAsString(row, i++);
        device.unit = ColumnAsString(row, i++);
        device.room = ColumnAsString(row, i++);

        device.current_value = ColumnAsString(row, i++);
        device.last_update = ColumnAsString(row, i++);
        device.state_bool = ColumnAsBool(row, i++);
        device.alias = ColumnAsString(row, i++);

        return device;
    }

    DeviceGroupAddress DeviceRepository::MapGroupAddressRow(MYSQL_ROW row)
    {
        DeviceGroupAddress ga;
        int i = 23;  // die ersten 23 Spalten (0-22) gehören zu smart_devices

        ga.id = row[i] ? std::atoi(row[i]) : 0; i++;
        ga.device_id = row[i] ? std::atoi(row[i]) : 0; i++;
        ga.function_name = ColumnAsString(row, i++);
        ga.ga = ColumnAsString(row, i++);
        ga.dpt = ColumnAsString(row, i++);
        ga.notes = ColumnAsString(row, i++);

        return ga;
    }

    std::vector<SmartDevice> DeviceRepository::QueryDevices(const std::string& whereClause)
    {
        std::string sql = std::string("SELECT ") + kDeviceColumns + ", " + kGroupColumns +
            " FROM smart_devices d "
            "LEFT JOIN device_group_addresses g ON g.device_id = d.id " +
            whereClause +
            " ORDER BY d.id";

        if (mysql_query(m_connection.Raw(), sql.c_str()))
        {
            throw DbException(std::string("Query fehlgeschlagen: ") + mysql_error(m_connection.Raw()));
        }

        MYSQL_RES* result = mysql_store_result(m_connection.Raw());
        if (!result)
        {
            throw DbException(std::string("mysql_store_result fehlgeschlagen: ") + mysql_error(m_connection.Raw()));
        }

        // Nach Geraete-ID gruppieren, da ein Geraet durch den LEFT JOIN
        // mehrfach vorkommen kann (einmal pro zusaetzlicher GA).
        std::map<int, SmartDevice> devicesById;
        std::vector<int> order;  // merkt sich die Reihenfolge (ORDER BY d.id)

        MYSQL_ROW row;
        while ((row = mysql_fetch_row(result)) != nullptr)
        {
            SmartDevice device = MapDeviceRow(row);

            auto it = devicesById.find(device.id);
            if (it == devicesById.end())
            {
                devicesById[device.id] = device;
                order.push_back(device.id);
                it = devicesById.find(device.id);
            }

            // Nur anhaengen, wenn der LEFT JOIN tatsaechlich eine GA-Zeile brachte
            // (bei device_group_addresses.id NULL gibt es keine passende Zeile)
            if (row[23] != nullptr)
            {
                it->second.extraGroupAddresses.push_back(MapGroupAddressRow(row));
            }
        }

        mysql_free_result(result);

        std::vector<SmartDevice> devices;
        for (int id : order)
        {
            devices.push_back(devicesById[id]);
        }
        return devices;
    }
    std::vector<SmartDevice> DeviceRepository::GetAll()
    {
        return QueryDevices("");
    }

    std::vector<SmartDevice> DeviceRepository::GetByProtocol(const std::string& protocol)
    {
        std::string where = "WHERE d.protocol = '" + Escape(protocol) + "'";
        return QueryDevices(where);
    }

    std::optional<SmartDevice> DeviceRepository::GetByGA(const std::string& ga)
    {
        std::string escaped = Escape(ga);
        std::string where =
            "WHERE d.ga = '" + escaped + "' OR "
            "d.ga_up = '" + escaped + "' OR "
            "d.ga_down = '" + escaped + "' OR "
            "d.ga_stop = '" + escaped + "' OR "
            "d.ga_position = '" + escaped + "' OR "
            "d.ga_status = '" + escaped + "' OR "
            "g.ga = '" + escaped + "'";

        auto devices = QueryDevices(where);
        if (devices.empty())
        {
            return std::nullopt;
        }
        return devices.front();
    }

    int DeviceRepository::InsertDevice(const SmartDevice& device)
    {
        std::string sql = "INSERT INTO smart_devices (ga, ga_up, ga_down, ga_stop, ga_position, ga_status, "
            "name, device_id, device_type, mqtt_topic, http_ip, protocol, category, "
            "enabled, icon, dpt, unit, room, current_value, last_update, state_bool, alias) VALUES ("
            + SqlValue(device.ga) + ", "
            + SqlValue(device.ga_up) + ", "
            + SqlValue(device.ga_down) + ", "
            + SqlValue(device.ga_stop) + ", "
            + SqlValue(device.ga_position) + ", "
            + SqlValue(device.ga_status) + ", "
            + SqlValue(device.name) + ", "
            + SqlValue(device.device_id) + ", "
            + SqlValue(device.device_type) + ", "
            + SqlValue(device.mqtt_topic) + ", "
            + SqlValue(device.http_ip) + ", "
            + SqlValue(device.protocol) + ", "
            + SqlValue(device.category) + ", "
            + (device.enabled ? "1" : "0") + ", "
            + SqlValue(device.icon) + ", "
            + SqlValue(device.dpt) + ", "
            + SqlValue(device.unit) + ", "
            + SqlValue(device.room) + ", "
            + SqlValue(device.current_value) + ", "
            + SqlValue(device.last_update) + ", "
            + (device.state_bool ? "1" : "0") + ", "
            + SqlValue(device.alias)
            + ")";
        if (mysql_query(m_connection.Raw(), sql.c_str()))
        {
            throw DbException(std::string("InsertDevice fehlgeschlagen: ") +
                mysql_error(m_connection.Raw()));
        }
        return static_cast<int>(mysql_insert_id(m_connection.Raw()));
	}
    void DeviceRepository::UpdateDevice(const SmartDevice& device)
    {
        std::string sql = "UPDATE smart_devices SET "
            "ga = " + SqlValue(device.ga) + ", "
            "ga_up = " + SqlValue(device.ga_up) + ", "
            "ga_down = " + SqlValue(device.ga_down) + ", "
            "ga_stop = " + SqlValue(device.ga_stop) + ", "
            "ga_position = " + SqlValue(device.ga_position) + ", "
            "ga_status = " + SqlValue(device.ga_status) + ", "
            "name = " + SqlValue(device.name) + ", "
            "device_id = " + SqlValue(device.device_id) + ", "
            "device_type = " + SqlValue(device.device_type) + ", "
            "mqtt_topic = " + SqlValue(device.mqtt_topic) + ", "
            "http_ip = " + SqlValue(device.http_ip) + ", "
            "protocol = " + SqlValue(device.protocol) + ", "
            "category = " + SqlValue(device.category) + ", "
            "enabled = " + (device.enabled ? "1" : "0") + ", "
            "icon = " + SqlValue(device.icon) + ", "
            "dpt = " + SqlValue(device.dpt) + ", "
            "unit = " + SqlValue(device.unit) + ", "
            "room = " + SqlValue(device.room) + ", "
            "current_value = " + SqlValue(device.current_value) + ", "
            "last_update = " + SqlValue(device.last_update) + ", "
            "state_bool = " + (device.state_bool ? "1" : "0") + ", "
            "alias = "+SqlValue(device.alias)+
            " WHERE id = "+std::to_string(device.id);
        if (mysql_query(m_connection.Raw(), sql.c_str()))
        {
            throw DbException(std::string("UpdateDevice fehlgeschlagen: ") +
                mysql_error(m_connection.Raw()));
        }
	}


    void DeviceRepository::UpdateDeviceState(int deviceId, const std::string& currentValue)
    {
        std::string sql =
            "UPDATE smart_devices SET "
            "current_value = " + SqlValue(currentValue) + ", " +
            "last_update = NOW() "
            "WHERE id = " + std::to_string(deviceId);

        if (mysql_query(m_connection.Raw(), sql.c_str()))
        {
            throw DbException(std::string("UpdateDeviceState fehlgeschlagen: ") + mysql_error(m_connection.Raw()));
        }
    }

    int DeviceRepository::InsertGroupAddress(const DeviceGroupAddress& ga)
    {
        std::string sql = "INSERT INTO device_group_addresses (device_id, function_name, ga, dpt, notes) VALUES ("
            + std::to_string(ga.device_id) + ", "
            + SqlValue(ga.function_name) + ", "
            + SqlValue(ga.ga) + ", "
            + SqlValue(ga.dpt) + ", "
            + SqlValue(ga.notes) +
            ")";
        if (mysql_query(m_connection.Raw(), sql.c_str()))
        {
            throw DbException(std::string("InsertGroupAddress fehlgeschlagen: ") +
                mysql_error(m_connection.Raw()));
        }
        return static_cast<int>(mysql_insert_id(m_connection.Raw()));
	}
    void DeviceRepository::UpdateGroupAdress(const DeviceGroupAddress& ga)
    {
        std::string sql = "UPDATE device_group_addresses SET "
            "device_id = " + std::to_string(ga.device_id) + ", "
            "function_name = " + SqlValue(ga.function_name) + ", "
            "ga = " + SqlValue(ga.ga) + ", "
            "dpt = " + SqlValue(ga.dpt) + ", "
            "notes = " + SqlValue(ga.notes) +
            " WHERE id = " + std::to_string(ga.id);
        if (mysql_query(m_connection.Raw(), sql.c_str()))
        {
            throw DbException(std::string("UpdateGroupAdress fehlgeschlagen: ") +
                mysql_error(m_connection.Raw()));
        }
	}
    void DeviceRepository::DeleteGroupAddress(int id)
    {
        std::string sql = "DELETE FROM device_group_addresses WHERE id = " + std::to_string(id);
        if (mysql_query(m_connection.Raw(), sql.c_str()))
        {
            throw DbException(std::string("DeleteGroupAddress fehlgeschlagen: ") +
                mysql_error(m_connection.Raw()));
        }
	}

 void DeviceRepository::DeleteDevice(int id)
    {
        std::string sql = "DELETE FROM smart_devices WHERE id = " + std::to_string(id);
        if (mysql_query(m_connection.Raw(), sql.c_str()))
        {
            throw DbException(std::string("DeleteDevice fehlgeschlagen: ") +
                mysql_error(m_connection.Raw()));
        }
 }
