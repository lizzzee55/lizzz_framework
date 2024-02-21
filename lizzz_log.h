#include "Singleton.h"
#include <string>
#include <vector>

struct log_t
{
	int fd;
	std::string name;
};

class lizzz_Log
{
	DECLARE_SINGLETON(lizzz_Log)
public:
	lizzz_Log();
	static int setLogdir(std::string dir);

	std::string current_dir;

	std::vector< log_t* > log_struct_list;
	
	void setLogName(std::string _name);
	void clear();
	void addLog(std::string data);
	log_t* getLogByName(std::string name);
	
private:
	std::string name;
	pthread_mutex_t mutex;
};


#pragma once

#include "lizzz_filesystem.h"
#include "lizzz_env.h"

inline lizzz_Log::lizzz_Log()
{
	pthread_mutex_init(&mutex, NULL);
	this->current_dir = lizzz_env::getDir();

	name = "default.log";
}

inline void lizzz_Log::setLogName(std::string _name)
{
	name = _name;
}

inline int lizzz_Log::setLogdir(std::string dir)
{
	lizzz_Log::Instance()->current_dir = dir;


	return 1;
}

inline log_t* lizzz_Log::getLogByName(std::string name)
{
	log_t* result = 0;

	for (int i = 0; i < log_struct_list.size(); i++)
	{
		if (log_struct_list[i]->name.find(name) == 0)
		{
			result = log_struct_list[i];
		}
	}
	return result;
}

inline void lizzz_Log::clear()
{
	pthread_mutex_lock(&mutex);
	log_t* logI = lizzz_Log::Instance()->getLogByName(name);
	if(!logI)
	{
		std::string filePath = lizzz_Log::Instance()->current_dir + "\\" + name;
		lizzz_filesystem::file_put_contents(filePath, "");
		this->addLog("Clear log");
	}
	pthread_mutex_unlock(&mutex);
}


inline void lizzz_Log::addLog(std::string data)
{
	pthread_mutex_lock(&mutex);

	data += "\n";
	printf(data.c_str());
	
#ifdef SP_WINDOWS
	OutputDebugStringA(data.c_str());
#endif

	//OutputDebugStringA(fileName.c_str());
	std::string filePath = lizzz_Log::Instance()->current_dir + "\\" + name;

	int fd = -1;

	log_t* logI = lizzz_Log::Instance()->getLogByName(name);
	if (!logI)
	{
		lizzz_env::createDirRecursive(filePath);

		fd = open(filePath.c_str(), O_RDWR | O_CREAT | O_APPEND, S_IWRITE | S_IREAD);

		logI = new log_t;
		logI->fd = fd;
		log_struct_list.push_back(logI);
	}


	int w_len = write(logI->fd, data.data(), data.length());
	pthread_mutex_unlock(&mutex);
	
}