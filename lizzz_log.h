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
	
	void clear(std::string fileName);
	void addLog(std::string fileName, std::string data);
	static void console(const char *str, ...);
	log_t* getLogByName(std::string name);
	
private:
	pthread_mutex_t mutex;
};


#pragma once

#include "lizzz_filesystem.h"
#include "lizzz_env.h"

inline lizzz_Log::lizzz_Log()
{
	pthread_mutex_init(&mutex, NULL);
	this->current_dir = lizzz_env::getDir();


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



inline void lizzz_Log::console(const char *fmt, ...)
{
	char str[1000];

	va_list args;
	va_start(args, fmt);
	vsprintf(str, fmt, args);
	va_end(args);

	printf(str);
#ifdef WIN32
	OutputDebugStringA(str);
#endif
	
	lizzz_Log::Instance()->addLog("log_console.txt", str);
}

inline void lizzz_Log::clear(std::string fileName)
{
	pthread_mutex_lock(&mutex);
	log_t* logI = lizzz_Log::Instance()->getLogByName(fileName);
	if(!logI)
	{
		std::string filePath = lizzz_Log::Instance()->current_dir + "\\" + fileName;
		lizzz_filesystem::file_put_contents(filePath, "");
		this->addLog(fileName, "Clear log");
	}
	pthread_mutex_unlock(&mutex);
}

inline void lizzz_Log::addLog(std::string fileName, std::string data)
{
	pthread_mutex_lock(&mutex);

	data += "\n";
	printf(data.c_str());
	
#ifdef WIN32
	OutputDebugStringA(data.c_str());
#endif

	//OutputDebugStringA(fileName.c_str());
	std::string filePath = lizzz_Log::Instance()->current_dir + "\\" + fileName;

	int fd = -1;

	log_t* logI = lizzz_Log::Instance()->getLogByName(fileName);
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





void LogClearAll()
{

}