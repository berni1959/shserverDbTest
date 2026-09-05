#include "DbConnection.h"

DbConnection::DbConnection(const std::string& host,
    const std::string& user,
    const std::string& password,
    const std::string& database,
    unsigned int port)
{
    m_conn = mysql_init(nullptr);
    if (!m_conn)
    {
        throw DbException("mysql_init() fehlgeschlagen");
    }

    my_bool verifyCert = 0;
    mysql_optionsv(m_conn, MYSQL_OPT_SSL_VERIFY_SERVER_CERT, &verifyCert);

    MYSQL* result = mysql_real_connect(
        m_conn,
        host.c_str(),
        user.c_str(),
        password.c_str(),
        database.c_str(),
        port,
        nullptr,
        0);

    if (!result)
    {
        std::string error = mysql_error(m_conn);
        mysql_close(m_conn);
        m_conn = nullptr;
        throw DbException("Verbindungsfehler: " + error);
    }

    mysql_set_character_set(m_conn, "utf8mb4");
}

DbConnection::~DbConnection()
{
    if (m_conn)
    {
        mysql_close(m_conn);
    }
}

DbConnection::DbConnection(DbConnection&& other) noexcept
    : m_conn(other.m_conn)
{
    other.m_conn = nullptr;
}

DbConnection& DbConnection::operator=(DbConnection&& other) noexcept
{
    if (this != &other)
    {
        if (m_conn)
        {
            mysql_close(m_conn);
        }
        m_conn = other.m_conn;
        other.m_conn = nullptr;
    }
    return *this;
}