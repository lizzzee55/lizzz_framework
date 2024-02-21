#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include <iostream>
 
using namespace rapidjson;

struct element
{
	std::string key;
	std::string val_s;
	int val_i;
	bool type;
};

class lizzz_json
{
public:

	lizzz_json();
	~lizzz_json();
	int parse(std::string data);
	std::string getString(std::string key);
	int getInt(std::string key);
	
	void setInt(std::string key, int val);
	void setString(std::string key, std::string val);
	std::string build();
	
private:
	Document document;
	std::vector< element > map;
};

#pragma once
#include "lizzz_filesystem.h"


//static 

inline lizzz_json::lizzz_json()
{

}

inline lizzz_json::~lizzz_json()
{
	
}

inline std::string lizzz_json::getString(std::string key)
{
	if(!document.HasMember(key.c_str())) return "";
	if(!document[key.c_str()].IsString()) return "";
	
	Value& a = document[key.c_str()];
	std::string value = a.GetString();
	return value;
}

inline int lizzz_json::getInt(std::string key)
{
	if(!document.HasMember(key.c_str())) return 0;
	if(!document[key.c_str()].IsInt()) return 0;
	
	Value& a = document[key.c_str()];
	int value = a.GetInt();
	return value;
}

inline void lizzz_json::setInt(std::string key, int val)
{
	element el;
	el.type = 0;
	el.key = key;
	el.val_i = val;
	map.push_back(el);
}

inline void lizzz_json::setString(std::string key, std::string val)
{
	element el;
	el.type = 1;
	el.key = key;
	el.val_s = val;
	map.push_back(el);
}

inline std::string lizzz_json::build()
{
	StringBuffer s;
    Writer<StringBuffer> writer(s);
    
    writer.StartObject();               // Between StartObject()/EndObject(), 
	
	for(int i = 0; i < map.size(); i++)
	{
		element el = map[i];
		writer.Key(el.key.c_str());
		if(el.type == 0)
		{
			writer.Int(el.val_i);
			//printf("int %d\r\n", el.val_i);
		}
		
		if(el.type == 1)
		{
			writer.String(el.val_s.c_str());
			//printf("string %s\r\n", el.val_s.c_str());
		}
	}
	
    writer.EndObject();

    // {"hello":"world","t":true,"f":false,"n":null,"i":123,"pi":3.1416,"a":[0,1,2,3]}
    //std::cout << s.GetString() << std::endl;
	
	
	return s.GetString();
}

inline int lizzz_json::parse(std::string data)
{

    document.Parse(data.c_str());
	
	if (document.HasParseError())
	{
		printf("error parse json\r\n");
        return 0;
	}
 
	printf("Json Parse succefull\r\n");
	
	return 1;
    
    
}
