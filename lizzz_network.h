#include <Singleton.h>
#include <string>
#include <vector>
#include <map>
#include "Processor.h"


class progress_data
{
public:
	size_t byte;
	size_t max_byte;
};

class lizzz_network
{
	DECLARE_SINGLETON(lizzz_network)
public:
	lizzz_network();
	~lizzz_network();
	lizzz_network* progress(void(*responce_callback)(std::string, std::string), void(*progress_callback)(progress_data));
	int lizzz_keep_alive_connect(std::string hostname, int port);
	
	lizzz_network* load(std::string url, std::string post);
	
	static int upload(std::string url, std::string post, std::string *data);
	static int uploadAndSave(std::string url, std::string path);
	
	std::string name;
	std::string jibril;
	std::string result;
	
	Processor* proc;
private:
	std::string url;
	void(*progress_callback)(progress_data) = NULL;
	void(*responce_callback)(std::string, std::string) = NULL;
//	int reader();
	int readerPageKeepAlive(int socket);
	int lizzz_keep_alive_close(std::string hostname, int port);

	
	bool need_close;

	
	std::map<std::string, int> socket_map;
	
private:
	pthread_mutex_t mutex;
};

#pragma once
#include "lizzz_socket.h"
#include "lizzz_functions.h"
#include "lizzz_url.h"
//#include "compressor.h"



inline lizzz_network::lizzz_network()
{
	pthread_mutex_init(&mutex, NULL);
	if(init_ws32())
	{
		lizzz_Log::Instance()->addLog("WS32 init success!");
	}
	this->proc = new Processor();
}

inline lizzz_network::~lizzz_network()
{
	delete this->proc;
}



inline lizzz_network* lizzz_network::progress(void(*responce_callback)(std::string, std::string), void(*progress_callback)(progress_data))
{
	this->responce_callback = responce_callback;
	this->progress_callback = progress_callback;
	return this;
}




inline int lizzz_network::readerPageKeepAlive(int socket)
{
	std::string header = "";
	std::string body = "";

	int startBody = 0;
	int contentLength = 0;
	std::string connectionType = "close";
	std::string transferEncoding = "";
	
	int endHeader = 0;
	
	char buffer[1024];
	
	while (true)
	{
		int n = recv(socket, buffer, sizeof buffer, MSG_NOSIGNAL);

		//lizzz_Log::Instance()->addLog("REsponce: " + std::string(buffer));
		//lizzz_Log::Instance()->addLog("REsponce: size " + std::string(buffer));
		
		if (n <= 0)
		{
			this->need_close = true;
			break;
		}

		header.append(buffer, n);
		
		if (contentLength > 0)
		{
			progress_data conf;
			conf.byte = header.length();
			conf.max_byte = contentLength;

			if(this->progress_callback != NULL)
				this->progress_callback(conf);
		}

		if (endHeader == 0 && (startBody = header.find("\r\n\r\n"))) {
			startBody += 4;
			
			body = header.substr(startBody, header.length() - startBody);

			endHeader = 1;
			proc->parseHeader(header);
			connectionType = proc->getHeader("connection");
			
			if(connectionType.find("keep-alive") != 0)
			{
				this->need_close = true;
			}
			
			transferEncoding = proc->getHeader("transfer-encoding");
			contentLength = proc->contentLength;
			
			
			header = header.substr(0, startBody - 2);
			break;

		}
	}

	//lizzz_Log::Instance()->addLog("Header: " + header);
	
	
	if (transferEncoding.find("chunked") == 0) {
		
		//printf("Chunk content\r\n");
		
		int chunkLen = 0;
		int startChunkBlock = 0;
		int endChunkBlock = 0;
		std::string chunk = "";

		while (true) {
			

			int n = endChunkBlock;

			chunk = "";

			int lenBody = body.length();
			
			
			while (n < lenBody) {
			
				if (body[n] == 13 && body[n + 1] == 10) { //Если найден чунк пересчитываем конец.
					
					chunkLen = strtol(chunk.c_str(), NULL, 16);
					
					startChunkBlock = n + 2;
					endChunkBlock = startChunkBlock + chunkLen;
					break;
				}

				chunk += (char)body[n];
				n++;

			}

			if (chunkLen == 0) { //если найден чунк с концом больше не загужаем.
				break;
			}
			//int chunkOffset = chunk.length() + 2;
			//lizzz_Log::Instance()->addLog("Chunk len: " + to_string(chunkOffset));


			while (endChunkBlock >= body.length()) {

				int len = recv(socket, buffer, sizeof buffer, MSG_NOSIGNAL); 
				if (len <= 0) {
					this->need_close = true;
					break;
				}
				
				if (len > 0) //если получили данные отправляем в калбэк
				{
					progress_data conf;
					conf.byte = body.length();
					conf.max_byte = 0;

					if(this->progress_callback != NULL)
						this->progress_callback(conf);
				}
				
				//lizzz_Log::Instance()->addLog("Add3: " + to_string(len));
				body.append(buffer, len);
			}

			endChunkBlock += 2;

		}
		
		
		body = proc->removeChunk(body);
	} 
	else if (contentLength > 0)
	{
		lizzz_Log::Instance()->addLog("ContentLength: " + to_string(contentLength) + " BodyLen: " + to_string(body.length()));
		
		int currentLen = body.length();
		if (currentLen < contentLength) {

			int n = 0;

			while (true) {
				n = recv(socket, buffer, sizeof buffer, MSG_NOSIGNAL); 

				if (n <= 0) {
					this->need_close = true;
					break;
				}
				
				if (n > 0) //если получили данные отправляем в калбэк
				{
					progress_data conf;
					conf.byte = body.length();
					conf.max_byte = contentLength;

					if(this->progress_callback != NULL)
						this->progress_callback(conf);
				}

				body.append(buffer, n);
				currentLen += n;

				if (currentLen >= contentLength) {
					break;
				}
			}
		}
		
		if (contentLength != body.length()) {
			lizzz_Log::Instance()->addLog("Error load data\r\n");
			return 0;
		}
	}
	
	//lizzz_Log::Instance()->addLog("Body: " + body);
		

	
	//lizzz_Log::Instance()->addLog("ResponceHeaders: " + proc->header);
	
	//lizzz_Log::Instance()->addLog("LastTime: " + proc->getHeader("lasttime"));


	std::string data = "";
	
	int res_header = 0;

	if (res_header == 0) {

		data = body;
	}
	else {
		data = header;
		data += "\r\n";
		data += body;
	}
	//delete proc;

	//lizzz_Log::Instance()->console("Res %s\r\n", data.c_str());

	progress_data conf;
	conf.byte = 0;
	conf.max_byte = contentLength;

	//lizzz_Log::Instance()->addLog("GetterName: " + this->name);

	//this->progress_callback(conf);
	if(this->responce_callback != NULL)
		this->responce_callback(data, this->name);
	
	this->result = data;
	
	return 1;
}



inline int lizzz_network::lizzz_keep_alive_connect(std::string hostname, int port)
{
	std::string key = hostname + ":" + to_string(port);
	
	int tmp_socket = socket_map[key];
	
	//alert(to_string(tmp_socket));
	if(tmp_socket)
	{
		return tmp_socket;
	}
	
	int socket = lizzz_socket::Connect(hostname, port);
	lizzz_Log::Instance()->addLog("gd");
	if(socket <= 0)
	{
		lizzz_Log::Instance()->addLog("Error connect to " + hostname + ":" + to_string(port));
		return 0;
	}
	
	lizzz_Log::Instance()->addLog("Success connect to " + hostname + ":" + to_string(port));
	//printf("Create socket %d %s:%d\r\n", socket, hostname.c_str(), port);
	
	pthread_mutex_lock(&mutex);
	socket_map[key] = socket;
	pthread_mutex_unlock(&mutex);
	
	this->need_close = false;
	
	return socket;
	
}

inline int lizzz_network::lizzz_keep_alive_close(std::string hostname, int port)
{
	pthread_mutex_lock(&mutex);
	
	std::string key = hostname + ":" + to_string(port);

	if(this->need_close)
	{
		
		
		std::map< std::string, int >::iterator it = socket_map.find(key);
		if(it != socket_map.end())
		{
			int socket = it->second;
			lizzz_Log::Instance()->addLog("Close socket " + to_string(socket));
			closesocket(socket);
			socket_map.erase(it);
		}

	}
	
	pthread_mutex_unlock(&mutex);
	return 1;
}

inline lizzz_network* lizzz_network::load(std::string url, std::string post)
{
	//url = "http://ali0.ru/empty.php";
	lizzz_Log::Instance()->addLog("Upload url: " + url);
	
	//parseUrl *parsed = new parseUrl(url);
	
	lizzz_url urll;
	urll.parse(url);
	
	//lizzz_Log::Instance()->addLog("host: " + urll.hostname + ":" + to_string(urll.port));
	
	
	int socket = lizzz_keep_alive_connect(urll.hostname, urll.port);// lizzz_socket::Connect(parsed->hostname, parsed->port);

	if (socket <= 0) 
	{
		return 0;
	}


	//std::vector< std::string > headers;
	//headers.push_back("User-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/87.0.4280.88 Safari/537.36");
	
	//parsed->method = "GET";
	//parsed->postData = "";
	//parsed->headers.push_back("User-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/87.0.4280.88 Safari/537.36");

	urll.setHeader("User-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/87.0.4280.88 Safari/537.36");
	std::string request = urll.buildRequest(post);//parsed->buildRequest();
	
	//lizzz_Log::Instance()->addLog("req:" + request);
	//lizzz_Log::Instance()->console("Req %s\r\n", request.c_str());

	int sended = send(socket, request.data(), request.length(), 0);
	//lizzz_Log::Instance()->addLog("send :" + to_string(sended));
	
	lizzz_network* result = NULL;
	
	//lizzz_Log::Instance()->addLog("asdasd:");
	if(this->readerPageKeepAlive(socket))
	{
		result = this;
	}
	//lizzz_Log::Instance()->addLog("asdasd2:");
	lizzz_keep_alive_close(urll.hostname, urll.port);
	//lizzz_Log::Instance()->addLog("asdasd3:");
	//delete parsed;
	
	return result;
}

inline int lizzz_network::upload(std::string url, std::string post, std::string *data)
{
	
	int result = 0;
	lizzz_network *model = lizzz_network::Instance();
	
	if(model->load(url, post) != NULL)
	{
		*data = model->result;
		result = 1;
	}
	//delete model;
	return result;
}


inline int lizzz_network::uploadAndSave(std::string url, std::string path)
{
	std::string name = lizzz_functions::getNameByPath(path);
	std::string data = "";
	if(lizzz_network::upload(url, "", &data))
	{
		lizzz_Log::Instance()->addLog("Success uploaded file: " + name + " bytes: " + to_string(data.length()));
		if(lizzz_filesystem::file_put_contents(path, data))
		{
			lizzz_Log::Instance()->addLog("Success save file: " + name);
			return 1;
		}
		
		lizzz_Log::Instance()->addLog("Error save file: " + name);
		return 0;
	}
	
	lizzz_Log::Instance()->addLog("Error upload file: " + name);
	return 0;
}