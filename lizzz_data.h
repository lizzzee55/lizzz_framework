#pragma once
#include <vector>
#include <map>
#include <string>

class Data
{
public:
	struct element
	{
		std::string key;
		std::string val_s;
		int val_i;
		bool type;
	};
	
	std::vector< element > map;
	
	int curr;
	Data()
	{
		curr = 0;
	}

	void set(std::string key, std::string val)
	{
		element el;
		el.type = 1;
		el.key = key;
		el.val_s = val;
		map.push_back(el);
	}
	
	void set(std::string key, int val)
	{
		element el;
		el.type = 0;
		el.key = key;
		el.val_i = val;
		map.push_back(el);
	}
	
	int getInt(std::string key)
	{
		int result = -1;
		for(int i = 0; i < map.size(); i++)
		{
			element el = map[i];
			if(el.type == 0 && el.key == key)
			{
				result = el.val_i;
				break;
			}
		}
		return result;
	}
	
	std::string getString(std::string key)
	{
		std::string result = "";
		for(int i = 0; i < map.size(); i++)
		{
			element el = map[i];
			if(el.type == 1 && el.key == key)
			{
				result = el.val_s;
				break;
			}
		}
		return result;
	}
	
	int size() { return map.size(); }
	
	int next(std::string& row)
	{
		
		row = "";
		if(map.size() == 0) return 0;
		
		//printf("get line size %d %d\r\n", curr, size());
		if(curr == map.size()) {
			curr = 0;
			return 0;
		}
		
		element el = map[curr];
		if(el.type == 0)
		{
			row = "`" + el.key + "` = " + to_string(el.val_i);
		}
		
		if(el.type == 1)
		{
			row = "`" + el.key + "` = '" + el.val_s + "'";
		}
		
		curr++;
		
		return 1;
	}
};
