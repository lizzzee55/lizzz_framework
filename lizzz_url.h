#include <string>
#include <vector>
#include <map>

class lizzz_url
{
public:

	std::string url;
	std::string dir;
	
	std::string hostname;
	int port;
	std::string protocol;
	std::string uri;
	std::string params;
	std::string hash;
	
	std::string method;
	std::string postData;
	
	std::map< std::string, std::string > postDataList;
	std::vector< std::string > headerList;


	int parse(std::string url);
	int parsePos(std::string body);

	void setHeader(std::string val);
	std::string buildRequest(std::string postData = "");
	
	static int parseGetRequest(std::map< std::string, std::string > &output, std::string uri);
};

#pragma once

#include "lizzz_functions.h"


inline int lizzz_url::parse(std::string url) //https://aferon.com/qwe/index.php?asd=123&zxc=321#bla
{
	std::vector< std::string > output;
	
	lizzz_functions::explode_twice(output, url, "#");
	if (output.size() > 1)
	{
		url = output[0];
		hash = output[1];
	}
	else {
		url = output[0];
	}
	
	
	lizzz_functions::explode_twice(output, url, "?");
	if (output.size() > 1)
	{
		dir = output[0];
		params = output[1];
	}
	else {
		dir = output[0];
	}
	
	parseGetRequest(getDataList, params);
	
	
	lizzz_functions::lizzz_explode(output, url, "/");
	
	port = 80;
	protocol = output[0];
	if(protocol.length() > 0)
	{
		if (protocol.find("https") == 0)
		{
			port = 443;
		}
	}
	
	
	int i = 1;
	
	if(output.size() > 2 && output[1].length() == 0)
	{
		hostname = output[2];
		
		std::vector< std::string > outputHostname;
		lizzz_functions::explode_twice(outputHostname, hostname, ":");
		if (outputHostname.size() > 1)
		{
			hostname = outputHostname[0];
			port = atoi(outputHostname[1].c_str());
		}
		else {
			hostname = outputHostname[0];
		}
		
		i = 3;
		
	}
	
	std::vector< std::string > path_part;
	for (; i < output.size(); i++)
	{
		path_part.push_back(output[i]);
	}

	uri = "/" + lizzz_functions::implode(path_part, "/");


	
	//lizzz_Log::Instance()->console("Protocol: %s Hostname %s port %d Uri %s Hash %s\r\n",
	//	protocol.c_str(),
	//	hostname.c_str(),
	//	port,
	//	uri.c_str(),
	//	hash.c_str());
	


	return 1;
}

inline std::string lizzz_url::get(std::string key)
{
	return getDataList[key];
}

inline std::string lizzz_url::post(std::string key)
{
	return postDataList[key];
}

inline void lizzz_url::setHeader(std::string val)
{
	headerList.push_back(val);
}

inline std::string lizzz_url::buildRequest(std::string postData)
{
	std::string method = "GET";
	if(postData.length() != 0)
	{
		method = "POST";
	}
	
	std::string result = method + " " + uri + " HTTP/1.1\r\n";
	
	if(port == 80)
		result += "Host: " + hostname + "\r\n";
	else
		result += "Host: " + hostname + ":" + to_string(port) + "\r\n";
	
	if (postData.length() > 0)
	{
		result += "Content-length: " + _itoa(postData.length()) + "\r\n";
	}

	result += "Connection: keep-alive\r\n";

	for(int i = 0; i < headerList.size(); i++)
	{
		result += headerList[i] + "\r\n";
	}

	result += "\r\n";
	result += postData;


	return result;
}

inline int lizzz_url::parseGetRequest(std::map< std::string, std::string > &output, std::string uri)
{
	output.clear();

	std::vector< std::string > lines;
	lizzz_functions::explode_twice(lines, uri, "?");
	if(lines.size() == 1)
		uri = lines[0];
	else
		uri = lines[1];
	
	if(uri.length() > 0)
	{
		std::vector< std::string > output_mass;
		lizzz_functions::lizzz_explode(output_mass, uri, "&");
		
		for (int i = 0; i < output_mass.size(); i++)
		{

			std::vector< std::string > outputPart;
			lizzz_functions::explode_twice(outputPart, output_mass[i], "=");
			if (outputPart.size() > 1)
			{
				std::string key = outputPart[0];
				std::string val = outputPart[1];
				
				output[key] = val;
			}
			else {
				std::string key = outputPart[0];
				output[key] = 1;
			}
			
		}
		
		return output.size();
	}

	return 0;
}

