#include "Db.h"
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
	static className* model()						            \
	{													        \
        return new className();                                 \
	}	                                                        \
    className* _getById(int id)                                 \
    {                                                           \
        if(this->_load(id))                                     \
        {                                                       \
            return this;                                        \
        }                                                       \
        delete this;                                            \
        return 0;                                               \
    }                                                           \
    int _clone(className* model)                                \
    {                                                           \
        for(int i = 0; i < model->_list.size(); i++)            \
        {                                                       \
            *(_list[i]->val) = *(model->_list[i]->val);         \
        }                                                       \
        for(int i = 0; i < model->_list_int.size(); i++)        \
        {                                                       \
            *(_list_int[i]->val) = *(model->_list_int[i]->val); \
        }                                                       \
       return 1;                                                \
    }       

class Model
{
public:
    std::string table;
    //std::map< std::string, std::string* > _assoc;
    //std::map< std::string, int* > _assoc_int;

    struct field_item
    {
        std::string key;
        std::string *val;
    };

    struct field_item_int
    {
        std::string key;
        int *val;
    };


    std::vector< field_item* > _list;
    std::vector< field_item_int* > _list_int;

    Model()
    {
        //this->db_name = db_name;
    }

    void _setTableName(std::string table)
    {
        this->table = table;
    }

    void _set(char *field, int *val)
    {
        *val = 0;
        field_item_int *item = new field_item_int;
        item->key = field;
        item->val = val;
        _list_int.push_back(item);

        //_assoc_int[field] = val;
    }

    void _set(char *field, std::string *val)
    {
        *val = "";

        field_item *item = new field_item;
        item->key = field;
        item->val = val;
        _list.push_back(item);

        //_assoc[field] = val;
    }

    field_item_int* _has_int(std::string field)
    {
        field_item_int *item = 0;
        for(int i = 0; i < _list_int.size(); i++)
        {
            if(_list_int[i]->key.find(field) == 0)
            {
                item = _list_int[i];
                break;
            }
        }
        return item;
    }

    field_item* _has(std::string field)
    {
        field_item *item = 0;
        for(int i = 0; i < _list.size(); i++)
        {
            if(_list[i]->key.find(field) == 0)
            {
                item = _list[i];
                break;
            }
        }
        return item;
    }
    
    int _save()
    {
        std::string sql = "INSERT INTO `"+this->table+"` ";

        std::vector< std::string > keys;
        std::vector< std::string > values;
        std::vector< std::string > keys_values;

        for(int i = 0; i < _list.size(); i++)
        {
            printf("key: %s\r\n", _list[i]->key.c_str());
            std::string val = "'" + *(_list[i]->val) + "'";
            std::string key_val = "`" + _list[i]->key + "` = " + val;
            keys.push_back("`" + _list[i]->key + "`");
            values.push_back(val);
            keys_values.push_back(key_val);
        }

        for(int i = 0; i < _list_int.size(); i++)
        {
            printf("key: %s\r\n", _list_int[i]->key.c_str());
            std::string val = utils::xitoa(*(_list_int[i]->val));
            keys.push_back("`" + _list_int[i]->key + "`");
            values.push_back(val);
            keys_values.push_back("`" + _list_int[i]->key + "` = " + val);
        }

        /*
        std::map< std::string, int* >::iterator it;
        for (it = _assoc_int.begin(); it != _assoc_int.end(); ++it)
        {
            printf("key: %s\r\n", it->first.c_str());
            std::string val = utils::xitoa(*(it->second));
            keys.push_back("`" + it->first + "`");
            values.push_back(val);
            keys_values.push_back("`" + it->first + "` = " + val);
        }
        
        std::map< std::string, std::string* >::iterator it_s;
        for (it_s = _assoc.begin(); it_s != _assoc.end(); ++it_s)
        {
            std::string val = "'" + *(it_s->second) + "'";
            std::string key_val = "`" + it_s->first + "` = " + val;
            keys.push_back("`" + it_s->first + "`");
            values.push_back(val);
            keys_values.push_back(key_val);
        }
        */

        sql += "(" + this->_implode(keys, ",") + ") VALUES (" + this->_implode(values, ",") + ") ON DUPLICATE KEY UPDATE " + this->_implode(keys_values, ",") + ";";

        
        int insert_id = Db::Instance()->insert(sql.c_str());
        printf("Sql: %s\r\nInsert_id: %d\r\n", sql.c_str(), insert_id);
        //exit(0);

        return insert_id;
    }
    
    int _load(int id)
    {
        std::string sql = "SELECT * FROM `"+this->table+"` where `id` = '"+utils::xitoa(id)+"' LIMIT 1";
        std::vector<std::map<std::string, std::string> > rows = Db::Instance()->query(sql.c_str());

        if(rows.size() == 0) return 0;

         //printf("Parse1 %d - %d\r\n", _assoc.size(), _assoc_int.size());


        std::vector< int > test;
        if(rows.size() == 1)
        {
            std::map< std::string, std::string > row = rows[0];

            std::map< std::string, std::string >::iterator it;
            for (it = row.begin(); it != row.end(); ++it)
            {
                std::string key = it->first;

                field_item *item = _has(key);
                if(item)
                {
                    //printf("key %s fined\r\n", key.c_str());
                    //
                    *(item->val) = it->second;
                }

                field_item_int *item2 = _has_int(key);
                if(item2)
                {
                    //printf("key_int %s fined\r\n", key.c_str());
                    *(item2->val) = atoi(it->second.c_str());
                }

                /*
                if(_assoc[key])
                {
                    *_assoc[key] = it->second;
                    test.push_back(1);
                    printf("key %s val %s\r\n", key.c_str(), (*_assoc[key]).c_str());
                }

                if(_assoc_int[key])
                {
                    *_assoc_int[key] = atoi(it->second.c_str());
                    test.push_back(1);
                    printf("key %s val %d\r\n", key.c_str(), (*_assoc_int[key]));
                }
                */

            }
        }

        //printf("Parse %d - %d\r\n", list.size(), list_int.size());
        //exit(0);
        return 1;
    }

    //static Template T create
    

    std::string _getJson()
    {
        std::string json;
        return json;
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