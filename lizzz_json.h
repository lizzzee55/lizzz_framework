#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include <iostream>
 
using namespace rapidjson;
class lizzz_json
{
public:

	lizzz_json();
	~lizzz_json();
	int parse(std::string data);
	std::string getString(std::string key);
	int getInt(std::string key);
	
private:
	Document document;
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
