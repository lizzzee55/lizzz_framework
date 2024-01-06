C++ for your first application

How to compile
g++ -w -DUNICODE -DWIN32 -s -static -static-libgcc -static-libstdc++ *.cpp  -o console.exe -I./../lizzz_framework -lws2_32 -lcrypt32

Hello world!

```
#include "includeBlock.h"
#include "lizzz_env.h"
#include "lizzz_log.h"
#include "lizzz_network.h"
#include "lizzz_json.h";

extern int __argc;
extern char** __argv;
static std::string globalParam;

std::string getParam()
{
	lizzz_Log::Instance()->addLog("Start: " + lizzz_env::getFileName());
	lizzz_Log::Instance()->addLog("Count argv: " + std::to_string(__argc));
	
	/*
    for (int i = 0; i < __argc; i++)
    {
		std::string ls(__argv[i]);
		lizzz_Log::Instance()->addLog("args: " + ls);
		if(i == 1)
			param = ls;
    }
	*/
	std::string param(__argv[__argc - 1]);
	
	lizzz_Log::Instance()->addLog("Param: " + param);
	globalParam = param;
	return param;
}

int main(int argc, char** argv)
{
	lizzz_Log::Instance()->setLogName("log.txt");
	lizzz_Log::Instance()->clear();
	lizzz_Log::Instance()->addLog("Log firset line!!");
	
	printf("params %s\r\n", getParam().c_str());
	
	std::string output_file = "out.txt";
	std::string url = "http://aferon.com/1.webm";
	std::string data = "";
	std::string post = "";
	
	if(!lizzz_upload(url, post, data))
	{
		lizzz_Log::Instance()->addLog("error network\r\n");
		return 0;
	}
	else
	{
		printf("Data recived: %d msg: \r\n%s\r\n", data.length(), data.c_str());
		if(lizzz_filesystem::file_put_contents(output_file, data) != -1)
		{
			lizzz_Log::Instance()->addLog("Success save file: " + output_file);
			return 1;
		}
	}
	return 0;
}
```
