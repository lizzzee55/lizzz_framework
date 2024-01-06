#include "Singleton.h"
#include <string>
#include <vector>

class lizzz_messager
{
	DECLARE_SINGLETON(lizzz_messager)
public:

	lizzz_messager();
	
	void setUid(const char* uid);
	void setDebug(bool val);
	void setUrl(const char* dir);
	int sendEvent(std::string name, std::string data);
	int sendStdout(std::string data);
	
	std::string builderRequest();
	
	int getUpdate(std::string &data);
	int uploadFileAndSave(std::string name, std::string saveFullPath);

private:
	const char* url;
	const char* uid;
	bool debug;

};

#pragma once
#include "lizzz_network.h"

inline lizzz_messager::lizzz_messager()
{
	this->url = NULL;
	this->uid = NULL;
	this->debug = false;
}

inline void lizzz_messager::setUid(const char* uid)
{
	this->uid = uid;
}

inline void lizzz_messager::setDebug(bool is_debug)
{
	this->debug = is_debug;
}

inline void lizzz_messager::setUrl(const char* url)
{
	this->url = url;
}

inline std::string lizzz_messager::builderRequest()
{
	std::string result;
	
	if (!this->url)
	{
		lizzz_Log::Instance()->addLog("Error URL: NULL" );
		return "";
	}

	result += std::string(this->url) + "update.php";
	
	if(this->uid)
		result += "?uid=" + std::string(this->uid);
	
	if(this->debug)
		result += "&debug=1";
	
	return result;
}

inline int lizzz_messager::sendEvent(std::string name, std::string data)
{
	std::string responce;
	std::string debugString = "";
	if(this->debug)
	{
		debugString = "&debug=1";
	}

	std::string mainUrl = this->builderRequest();
	if (mainUrl.length() == 0)
	{
		return 0;
	}

	std::string url = this->builderRequest() + "&action=1&event=" + name + "&data=" + data;
	if(lizzz_upload(url, "", responce))
	{
		return 1;
	}
	return 0;
}

inline int lizzz_messager::sendStdout(std::string data)
{
	std::string responce;
	std::string url = "http://aferon.com:8090/send?message=ok";
	if(lizzz_upload(url, data, responce))
	{
		return 1;
	}
	return 0;
}

inline int lizzz_messager::getUpdate(std::string &data)
{
	std::string url = this->builderRequest();
	return lizzz_upload(url, "", data);
}


inline int lizzz_messager::uploadFileAndSave(std::string name, std::string saveFullPath)
{
	std::string url = std::string(this->url) + name;
	return lizzz_upload_file(url, saveFullPath);
}

