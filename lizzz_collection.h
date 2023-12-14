#include <string>
#include <Singleton.h>

template <class T>
class lizzz_collection
{
	DECLARE_SINGLETON(lizzz_collection)
	
	
public:
	lizzz_collection();
	void push(T&);
	T get(std::string key);
	void clear();
	
	//T* data_t() { return list[0]; }
	
private:
	std::vector< T > list;
};

#pragma once

template <class T>
inline lizzz_collection<T>::lizzz_collection()
{
	//this->list = new std::vector< T >;
}

template <class T>
inline T lizzz_collection<T>::get(std::string key)
{
	T result = NULL;
	for (int i = 0; i < list; i++)
	{
		T tmp = list[i];
		if (tmp->key.find(key) == 0)
		{
			result = tmp;
		}
	}
	return result;
}

template <class T>
inline void lizzz_collection<T>::clear()
{
	std::vector< T > newList;
	for(int i = 0; i < this->list.size(); i++)
	{
		T tmp = list->at(i);
		if(!tmp->is_need_delete)
		{
			newList.push_back(tmp);
		}
	}
	this->list.clear();
	this->list = newList;
}

template <class T>
inline void lizzz_collection<T>::push(T& val)
{
	val->key;
	list.push_back(val);
}
