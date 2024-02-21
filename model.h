#include "lizzz_db.h"
#include "lizzz_json.h"
#include "lizzz_data.h"
#include <vector>
#include <string>
#include <map>


#pragma once
/*
    Author jibril/lizzz/slaid
*/

#include <string>

#define MODEL(className)			        		            \
public:													        \
	static className* model()									\
	{															\
		static className* myInstance = NULL;					\
		if(!myInstance)											\
		{														\
			myInstance = new className();						\
		}														\
		return myInstance;										\
	}															\
    className* findByPk(int id)                                  \
    {                                                           \
        if(this->_findByPk(id))                                     \
        {                                                       \
            return this;                                        \
        }                                                       \
        return 0;                                               \
    }                                                           \
	className* findByAttribute(Data *data) \
    {                                                           \
        if(this->_findByAttribute(data))                                  \
        {                                                       \
            return this;                                        \
        }                                                       \
        return 0;                                               \
    }                                                           \
																\
//    int _clone(className* model)                                \
//    {                                                           \
//        for(int i = 0; i < model->_list.size(); i++)            \
//        {                                                       \
//            *(_list[i]->val) = *(model->_list[i]->val);         \
//        }                                                       \
//        for(int i = 0; i < model->_list_int.size(); i++)        \
//        {                                                       \
//            *(_list_int[i]->val) = *(model->_list_int[i]->val); \
//        }                                                       \
//       return 1;                                                \
//    }

class Model
{
public:

    Model()
    {
        //this->db_name = db_name;
    }

    struct element
	{
		std::string key;
		std::string *val_s;
		int *val_i;
		bool type;
	};

	std::vector< element > _list;

	std::string table;
    void _setTableName(std::string _table)
    {
		//_list.resize(30);
		//std::vector< element > test(30);
        table = _table;
    }

    void _link(std::string key, int &val)
    {
		element el;
		el.type = 0;
		el.key = key;
		el.val_i = &val;
		_list.push_back(el);
    }

    void _link(std::string key, std::string &val)
    {
        element el;
		el.type = 1;
		el.key = key;
		el.val_s = &val;
		_list.push_back(el);
    }

	int _findByPk(int id)
	{
		_clear();
		std::string sql = "select * from `" + table + "` where `id` = " + to_string(id) + " limit 1;";
		db_resource* resource = db_lizzz::Instance()->query(sql.c_str());

		std::map< std::string, std::string > output;
		if(db_lizzz::Instance()->getData(output, resource))
		{
			_fillData(output);
			return 1;
		}

		return 0;
	}

	int _findByAttribute(Data *data)
	{
		_clear();
		std::string criteria = "";
		std::string line;
		while(data->next(line))
		{
			criteria += line + " and ";
		}
		criteria = criteria.substr(0, criteria.length() - 5);

		std::string sql = "select * from `" + table + "` where " + criteria + " limit 1;";
		//printf("line: %s\r\n", sql.c_str());
		db_resource* resource = db_lizzz::Instance()->query(sql.c_str());

		std::map< std::string, std::string > output;
		if(db_lizzz::Instance()->getData(output, resource))
		{
			_fillData(output);
			return 1;
		}

		return 0;
	}

	void _clear()
	{
		for(int l = 0; l < _list.size(); l++)
		{
			element el = _list[l];
			if(el.type)
			{
				*(el.val_s) = "";
			} else {
				*(el.val_i) = -1;
			}

			//if(key.find(_list[l]
		}
	}

	int _fillData(std::map< std::string, std::string > &output)
	{
		printf("size output %d\r\n", output.size());
		std::string key, val;
		std::map< std::string, std::string >::iterator it = output.begin();

		for (int i = 0; it != output.end(); it++, i++)
		{
			key = it->first;
			val = it->second;
			for(int l = 0; l < _list.size(); l++)
			{
				element el = _list[l];
				if(el.type)
				{
					if(key.find(el.key) == 0)
					{
						*(el.val_s) = val;
						//printf("el key %s val %s\r\n", el.key.c_str(), val.c_str());
					}

				} else {
					if(key.find(el.key) == 0)
					{
						int ival = atoi( val.c_str() );
						*(el.val_i) = ival;
						//printf("el key %s val %d\r\n", el.key.c_str(), ival);
					}
				}

				//if(key.find(_list[l]
			}
			//cout << i << ") Ключ " << it->first << ", значение " << it->second << endl;
		}


	}

    int _save()
    {

        std::string sql = "INSERT INTO `" + table + "` ";

        std::vector< std::string > keys;
        std::vector< std::string > values;
        std::vector< std::string > keys_values;

		for(int l = 0; l < _list.size(); l++)
		{
			element el = _list[l];
			std::string key = el.key;
			if(el.type)
			{
				std::string val = "'" + *(el.val_s) + "'";
				std::string key_val = "`" + key + "` = " + val;
				keys.push_back("`" + key + "`");
				values.push_back(val);
				keys_values.push_back(key_val);


			} else {
				int ival = *(el.val_i);
				std::string val = to_string(ival);
				keys.push_back("`" + key + "`");
				values.push_back(val);
				keys_values.push_back("`" + key + "` = " + val);
			}

		}


        sql += "(" + this->_implode(keys, ",") + ") VALUES (" + this->_implode(values, ",") + ") ON DUPLICATE KEY UPDATE " + this->_implode(keys_values, ",") + ";";

        int insert_id = db_lizzz::Instance()->insert(sql.c_str());
		printf("insert_id: %d\r\n", insert_id);

        return insert_id;

    }


    std::string getJson()
    {

		lizzz_json json;


		for(int l = 0; l < _list.size(); l++)
		{
			element el = _list[l];
			if(el.type)
			{
				std::string val = *(el.val_s);
				json.setString(el.key, val);


			} else {
				int ival = *(el.val_i);
				json.setInt(el.key, ival);

			}

			//if(key.find(_list[l]
		}

        return json.build();
    }

    
    
private:
    
    
protected:
    std::string _implode(std::vector<std::string> arr, const char *delim)
	{
		std::string result;
		for (int i = 0; i < arr.size(); i++)
		{
			result += arr[i] + delim;
		}

		result = result.substr(0, result.length() - strlen(delim));

		return result;
	}
};