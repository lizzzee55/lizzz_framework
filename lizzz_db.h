#include "Singleton.h"

//sql 8.0
//#include <boost/scoped_ptr.hpp>
//#include "SQLAPI.h"
//#include <mysql/jdbc.h>

//sql 5.7
#include "mysql_connection.h"
#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>

class db_resource
{
public:
	std::vector< std::string > field_name;
	int num_fields;
	sql::ResultSet* res;
	
	db_resource()
	{
		num_fields = 0;
	}
};

class db_lizzz
{
	DECLARE_SINGLETON(db_lizzz);
public:
	db_lizzz();
	int connect(std::string host, std::string user, std::string pass, std::string database, int port = 3306);
	db_resource* query(const char *query);
	int insert(const char *query);
	int update(const char *query);

	int getData(std::map< std::string, std::string > &output, db_resource* resource);

	int close();

private:
	sql::Driver* driver;
	sql::Connection* con;
	
	sql::Statement* stmt;
	sql::ResultSetMetaData* meta;
	
	
	pthread_mutex_t mutex;
};

#pragma once
inline db_lizzz::db_lizzz()
{
	pthread_mutex_init(&mutex, NULL);
	
	std::string host = "185.43.6.164";
	int port = 3306;
	std::string user = "lizzzee2";
	std::string pass = "BiG162534";
	std::string database = "exe_launcher";


	connect(host, user, pass, database, port);
	
}

inline int db_lizzz::connect(std::string host, std::string user, std::string pass, std::string database, int port)
{
	driver = get_driver_instance();
    con = driver->connect(host, user, pass);
    /* Connect to the MySQL test database */
    con->setSchema(database);

	printf("is alive %d\r\n", con->isValid());

	return 1;
}

inline int db_lizzz::check()
{
	if(!con->isValid())
		con->reconnect();
	return 1;
}

inline int db_lizzz::close()
{
	if(con)
	{
		con->close();
		delete con;
		return 1;
	}
	return 0;
}

#ifdef DEBIAN
/*

//for mysql connector 8.0.* /debian
inline std::vector<std::map<std::string, std::string> > db_lizzz::query(const char *query)
{

	//printf("Sql: %s\r\n", query);

	std::vector<std::map<std::string, std::string>> result;


	if (mysql_query(&mysql, query) > 0) // ������. ���� ������ ���, �� ���������� ������
	{

		// ���� ���� ������, ...
		printf("Err: %s", mysql_error(&mysql));  // ... ������ ��
		return result; // � �������� ������
	}


	MYSQL_FIELD* field;

	res = mysql_store_result(&mysql); // ����� ���������,
	int num_fields = mysql_num_fields(res); // ���������� �����
	int num_rows = mysql_num_rows(res); // � ���������� �����.

										//printf("nums %d = %d\r\n", num_fields, num_rows);

	std::vector<std::string> field_name;

	for (int i = 0; i < num_fields; i++) // ������� �������� �����
	{
		field = mysql_fetch_field_direct(res, i); // ��������� �������� �������� ����
		field_name.push_back(field->name);
		//printf("| %s |", field->name);
	}

	//printf("\n");



	MYSQL_ROW row; // ������ ����� ������� ������

	for (int i = 0; i < num_rows; i++) // ����� �������
	{
		std::map<std::string, std::string> line;

		row = mysql_fetch_row(res); // �������� ������

		for (int l = 0; l < num_fields; l++)
		{
			if (row[l])
			{
				line.insert(std::pair<std::string, std::string>(field_name[l], row[l]));
			}
			else {
				line.insert(std::pair<std::string, std::string>(field_name[l], ""));
			}


			//line[field_name[l]] = row[l];

			//printf("| %s |", row[l]); // ������� ����

		}
		result.push_back(line);


		//printf("\n");
	}

	printf("Count records = %d\r\n", num_rows);
	mysql_free_result(res);


	return result;

}
*/
#endif



//mysql connector 5.7 yum install mysql-connector-odbc
inline db_resource* db_lizzz::query(const char *query)
{
	printf("Sql: %s\r\n", query);
	
	db_resource *resource = NULL;

	try {
		resource = new db_resource;

		//printf("QuerySql request\r\n");
		stmt = con->createStatement();
		
		resource->res = stmt->executeQuery(query);

		meta = resource->res->getMetaData();
		resource->num_fields = meta->getColumnCount();
		//int num_rows = resource->res->rowsCount();

		std::string field;

		for (int i = 0; i < resource->num_fields; i++) // ������� �������� �����
		{
			field = meta->getColumnName(i + 1);
			resource->field_name.push_back(field);

		}
		
		

		if(resource->field_name.size() == 0) 
		{
			delete resource->res;
			delete resource;
			resource = NULL;
		}

		//delete meta;
		//printf("sql=>row_ok\r\n");
	
		// con->close();
		//delete meta;
		//delete res;
		//delete stmt;
		// delete con;
		//printf("sql=>exit\r\n");
	}
	catch (sql::SQLException& e) {
		std::cout << "# ERR: SQLException in " << __FILE__;
		std::cout << "(" << __FUNCTION__ << ") on line "
			<< __LINE__ << std::endl;
		std::cout << "# ERR: " << e.what();
		std::cout << " (MySQL error code: " << e.getErrorCode();
		std::cout << ", SQLState: " << e.getSQLState() << " )" << query << std::endl;
	}
	//printf("Sql finish\r\n");


	return resource;
}

inline int db_lizzz::getData(std::map< std::string, std::string > &output, db_resource* resource)
{
	output.clear();
	
	if (resource->res->next()) {
		for (int l = 0; l < resource->num_fields; l++)
		{
			std::string key = resource->field_name[l];
			std::string val = resource->res->getString(l + 1);

			output[key] = val;

		}

		return 1;
	}
	
	return 0;
}


inline int db_lizzz::insert(const char *query)
{
	printf("Sql: %s\r\n", query);
	int result = 0;

	try {

		sql::Statement* stmt;
		stmt = con->createStatement();
		stmt->execute(query);
		delete stmt;

	}
	catch (sql::SQLException& e) {
		std::cout << "# ERR: SQLException in " << __FILE__;
		std::cout << "(" << __FUNCTION__ << ") on line "
			<< __LINE__ << std::endl;
		std::cout << "# ERR: " << e.what();
		std::cout << " (MySQL error code: " << e.getErrorCode();
		std::cout << ", SQLState: " << e.getSQLState() << " )" << std::endl;
	}

	std::map< std::string, std::string > output;
	db_resource* resource = db_lizzz::query("SELECT LAST_INSERT_ID() AS id");
	
	while(getData(output, resource))
	{
		//int size = resource->field_name.size();
		//for(int i = 0; i < size; i++)
		//{
		//	printf("lastid %s <size %d> id %s\r\n", resource->field_name[i].c_str(), size, output[""].c_str());
		//}
		result = 1;//atoi(it->second.c_str());
	}



	return result;
}

inline int db_lizzz::update(const char *query)
{
	printf("Sql: %s\r\n", query);
	int result = 0;

	try {

		sql::Statement* stmt;
		stmt = con->createStatement();
		stmt->execute(query);
		delete stmt;

	}
	catch (sql::SQLException& e) {
		std::cout << "# ERR: SQLException in " << __FILE__;
		std::cout << "(" << __FUNCTION__ << ") on line "
			<< __LINE__ << std::endl;
		std::cout << "# ERR: " << e.what();
		std::cout << " (MySQL error code: " << e.getErrorCode();
		std::cout << ", SQLState: " << e.getSQLState() << " )" << std::endl;
	}

	std::map< std::string, std::string > output;
	db_resource* resource = db_lizzz::query("SELECT LAST_INSERT_ID() AS id");
	
	while(getData(output, resource))
	{
		result = 1;//atoi(it->second.c_str());
	}


	return result;
}


