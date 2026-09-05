#pragma once
# include <mysql/mysql.h>
#include <string>
#include <stdexcept>

class DbConnection
{

public: DbConnection(
	const std::string& host,
	const std::string& user,
	const std::string& password,
	const std::string& database,
	unsigned int port = 3306);
	  ~DbConnection();

	  DbConnection(const DbConnection&) = delete;
	  DbConnection& operator=(const DbConnection&) = delete;

	  DbConnection(DbConnection&& other) noexcept;
	  DbConnection& operator=(DbConnection&& other) noexcept;	

	  MYSQL* Raw() const { return m_conn; }
	
	private:
		MYSQL* m_conn = nullptr;
};
class DbException : public std::runtime_error
{
	public:
		DbException(const std::string& message) : std::runtime_error(message) {}	
};