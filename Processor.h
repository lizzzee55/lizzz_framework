#include <algorithm>
#include "Singleton.h"
#include <vector>
#include <map>


class Processor {

public:
	std::string firstLine;

	std::string html;
	std::string header;
	std::string body;


	/* most key headers */
	std::string method;
	std::string uri;
	std::string protocol;

	bool is_post;
		
	/* types */
	std::string contentType;
	std::string contentEncode;
	std::string transferEncoding;
	std::string connection;
	std::string host;
	int port;
	int contentLength;
	int code;
	std::string responce_status;

	int is_gzip;
	int is_chunked;

	std::string referer;

	Processor();

	int splitHtml();

	int parseHeader(std::string header);
	int parseUri(std::string uri);
	
	int parse(std::string html);

	std::string getHeader(std::string key);
	std::string removeChunk(std::string body);

	void parseData();
	int parseGetRequest(std::map< std::string, std::string > &output, std::string str);
	//std::string get(std::string name);

	//std::map <std::string, std::string> dataList;
	std::map <std::string, std::string> headerList;

	std::string tmp;

protected:
	void serializeHeader();
	int parseFirstLine();
	
};

#pragma once
#include "lizzz_functions.h"
#include "lizzz_url.h"
#include <sstream>

inline Processor::Processor()
{

	this->is_gzip = 0;
	this->is_chunked = 0;
	this->contentLength = -1;
};




inline std::string Processor::getHeader(std::string key) {

	return this->headerList[key];
};

inline int Processor::parseHeader(std::string header) {

	this->headerList.clear();
	std::string line;

	std::stringstream fd;
	fd << header;

	int numLine = 0;
	while (getline(fd, line))
	{
		if (line.length() == 0)
			break;

		if (numLine == 0)
		{
			this->firstLine = line;
		}
		else
		{


			int posDel = line.find(":");
			if (posDel != 0 && posDel != std::string::npos)
			{
				std::string key = line.substr(0, posDel);
				std::string val = line.substr(posDel + 1, line.length() - posDel - 1);

				lizzz_functions::ft_tolower(key);
				lizzz_functions::trim(key);
				lizzz_functions::trim(val);

				this->headerList[key] = val;
			}
		}
		numLine++;
	}

	return this->parseFirstLine();


}

inline int Processor::parseFirstLine()
{
	std::vector<std::string> arr;
	lizzz_functions::lizzz_explode(arr, this->firstLine, " ");

	bool is_request = 0;
	bool is_responce = 0;
	
	std::string firstArg = arr[0];
	if(firstArg.find("HTTP/") == 0)
	{
		is_responce = 1;
	} else {
		is_request = 1;
	}

	if(is_responce)
	{
		this->protocol = arr[0];
		this->code = atoi(arr[1].c_str());
		this->responce_status = arr[2];
		
	}

	if(is_request)
	{
		this->method = arr[0];
		this->uri = arr[1];
		this->protocol = arr[2];
		this->parseUri(this->uri);
	}

	this->serializeHeader();
	if(is_request)
	{
		printf("RequestMethod: %s Connection: %s ContentLength: %d Uri: %s\r\n", this->method.c_str(), this->connection.c_str(), contentLength, this->uri.c_str());
	}

	return 1;
}


inline int Processor::parseUri(std::string uri)
{
	
	return 1;
}

inline int Processor::splitHtml()
{
	int pos = this->html.find("\r\n\r\n");
	if (pos != std::string::npos)
	{
		this->header = this->html.substr(0, pos);
		this->body = this->html.substr(pos + 4, this->html.length());
		return 1;
	}
	return 0;
}


inline int Processor::parse(std::string html) {
	this->html = html;
	if (this->splitHtml())
	{
		this->parseHeader(this->header);


		return 1;
	}

	return 0;
};


inline void Processor::serializeHeader()
{
	this->contentType = this->getHeader("content-type");
	this->contentEncode = this->getHeader("content-encoding");
	this->transferEncoding = this->getHeader("transfer-encoding");
	this->contentLength = atoi(this->getHeader("content-length").c_str());
	this->connection = this->getHeader("connection");
	this->host = this->getHeader("host");
	this->port = 80;
	
	//lizzz_Log::Instance()->addLog("transferEncoding: " + transferEncoding);

	if (this->host.length() > 0)
	{
		int posDel = host.find(":");
		if (posDel != std::string::npos)
		{
			std::string portS = host.substr(posDel + 1, this->host.length() - posDel + 1);
			this->port = atoi(portS.c_str());
			this->host = this->host.substr(0, posDel);
		}
	}


	if (this->contentEncode.find("gzip") != std::string::npos)
	{
		this->is_gzip = 1;
	}

	if (this->transferEncoding.find("chunked") != std::string::npos)
	{
		this->is_chunked = 1;
	}
	

};

inline std::string Processor::removeChunk(std::string body) {

	int chunkPart = 0;
	int chunkLen = 0;
	int startChunkBlock = 0;
	int endChunkBlock = 0;

	std::string block;
	int posBlock = 0;
	std::string bodyNoChunk;

	std::string chunk = "";
	//printf("Read chunk | allLength %d\r\n", this->responce->body);


	while (true) {
		int n = (endChunkBlock) ? endChunkBlock : 0;

		//printf("start chunk block %d %d htmllen\r\n", n, pSvcInfo->html.length());

		if (n >= body.length()) {
			//printf("break 1\r\n");
			break;
		}

		chunk = "";

		while (true) {
			if (body[n] == 13 && body[n + 1] == 10) {
				chunkLen = strtol(chunk.c_str(), NULL, 16);
				startChunkBlock = n + 2;
				endChunkBlock = startChunkBlock + chunkLen;
				break;
			}
			chunk += (char)body[n];
			n++;

			if (n >= body.length()) {
				//printf("break 2\r\n");
				break;
			}
		}

		//printf("Chunk(%d) %s %d (%d - %d)\r\n", chunk.length(), chunk.c_str(), chunkLen, startChunkBlock, endChunkBlock);

		if (chunkLen == 0) {
			break;
		}

		for (int i = startChunkBlock; i < endChunkBlock; i++) {
			bodyNoChunk += (char)body[i];
		}

		endChunkBlock += 2;
	}

	return bodyNoChunk;

}