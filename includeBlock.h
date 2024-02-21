#pragma once
#pragma warning(disable : 4996)

#define CRYPTOPP_NO_CXX11

#if defined( _WINDOWS ) || defined( _SP_WINDOWS ) || defined( SP_WINDOWS ) || defined( _WIN64 )

#	define SP_WINDOWS

#elif defined( unix ) || defined( __unix__ ) || defined( __unix )

#	define SP_UNIX

#	if defined( ANDROID ) || defined( __ANDROID__ )
#		define SP_ANDROID
#	elif defined( linux ) || defined( __linux__ ) || defined( __linux )
#		define SP_LINUX
#	endif // ANDROID / linux

#else
#	error Unknown platform!
#endif

#ifdef SP_WINDOWS
#include <winsock2.h>
#pragma comment(lib,"ws2_32.lib")
#pragma comment(lib,"crypt32.lib")
#include <ws2tcpip.h>
#include "windows.h"
#include <tchar.h>
#include "io.h"
#include <sys/stat.h>

#define SHUT_RDWR 2
#define MSG_NOSIGNAL 0

#else
#define WSAGetLastError() 1

//#define _XOPEN_SOURCE_EXTENDED 1 //#define _OE_SOCKETS //
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/time.h>
#include <poll.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <wait.h>
#include <cstring>
#include <netdb.h>
#include <errno.h>
#include <pthread.h>
#include <sstream>

#define SOCKET_ERROR -1
#define INVALID_SOCKET 0

extern char **environ;

typedef unsigned long DWORD;
typedef void* (*LPTHREAD_START_ROUTINE)(void*);
#define FALSE 0
#define TRUE 1
typedef void* LPVOID;

//typedef pthread_t HANDLE;

#define HANDLE pthread_t

#define closesocket(s) close(s)
#define Sleep(s) usleep(s * 1000)
#define SOCKET int

#endif

#include <string>
#include <vector>
#include <fcntl.h>

#ifdef SP_WINDOWS

#include <mutex>


#define pthread_mutex_t CRITICAL_SECTION
#define pthread_mutex_init(x, y) InitializeCriticalSection(x)
#define pthread_mutex_lock(x) EnterCriticalSection(x)
#define pthread_mutex_unlock(x) LeaveCriticalSection(x)
#define pthread_mutex_destroy(x) DeleteCriticalSection(x)



const char *lizzz_inet_ntop(int af, const void *src, char *dst, socklen_t size)
{
	struct sockaddr_storage ss;
	unsigned long s = size;

	ZeroMemory(&ss, sizeof(ss));
	ss.ss_family = af;

	switch (af) {
	case AF_INET:
		((struct sockaddr_in *)&ss)->sin_addr = *(struct in_addr *)src;
		break;
	case AF_INET6:
		((struct sockaddr_in6 *)&ss)->sin6_addr = *(struct in6_addr *)src;
		break;
	default:
		return NULL;
	}
	/* cannot direclty use &size because of strict aliasing rules */
	return (WSAAddressToStringA((struct sockaddr *)&ss, sizeof(ss), NULL, dst, &s) == 0) ?
		dst : NULL;
}

int lizzz_inet_pton(int af, const char *src, void *dst)
{
	struct sockaddr_storage ss;
	int size = sizeof(ss);
	char src_copy[INET6_ADDRSTRLEN + 1];

	ZeroMemory(&ss, sizeof(ss));
	/* stupid non-const API */
	strncpy(src_copy, src, INET6_ADDRSTRLEN + 1);
	src_copy[INET6_ADDRSTRLEN] = 0;

	if (WSAStringToAddressA(src_copy, af, NULL, (struct sockaddr *)&ss, &size) == 0) {
		switch (af) {
		case AF_INET:
			*(struct in_addr *)dst = ((struct sockaddr_in *)&ss)->sin_addr;
			return 1;
		case AF_INET6:
			*(struct in6_addr *)dst = ((struct sockaddr_in6 *)&ss)->sin6_addr;
			return 1;
		}
	}
	return 0;
}

#else
#define lizzz_inet_ntop(s1, s2, s3, s4) inet_ntop(s1, s2, s3, s4)
#define lizzz_inet_pton(s1, s2, s3) inet_pton(s1, s2, s3)
#endif



#ifndef MAX
#define MAX(x, y) ((x) > (y) ? (x) : (y))
#endif
#undef PTHREAD_STACK_MIN
#define PTHREAD_STACK_MIN 64*1024

size_t stacksz = MAX(8192, PTHREAD_STACK_MIN);

HANDLE createThread(LPTHREAD_START_ROUTINE fnRoutine, LPVOID lpParameter)
{

#ifdef SP_WINDOWS
    DWORD m_dwThreadId;
    return ::CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)fnRoutine, lpParameter, 0, &m_dwThreadId);
#else
    HANDLE pt;

    pthread_attr_t* a = 0, attr;
    if (pthread_attr_init(&attr) == 0) {
        a = &attr;
        pthread_attr_setstacksize(a, stacksz);
    }
    if (pthread_create(&pt, a, fnRoutine, lpParameter) != 0)
    {
        printf("pthread_create failed. OOM?\n");
        exit(0);
    }

    //printf("Create Ptr %d\r\n", pt);

    if (a) pthread_attr_destroy(&attr);

    return pt;

    //int ret = pthread_create(&m_hThread, NULL, fnRoutine, lpParameter);
    //return m_hThread;
#endif
}


void joinThread(HANDLE pt)
{
    if(pt)
    {
#ifdef SP_WINDOWS
        ::WaitForSingleObject(pt, INFINITE);
#else
        pthread_join(pt, 0);
#endif
    }

}

static std::string _itoa(int val)
{
	//printf("val %d\r\n", val);
	char buf[128];
	memset(buf, 0, 128);
	sprintf(buf, "%d", val);
	std::string result(buf, strlen(buf));
	return result;
}

#include <stdarg.h>
void LOG(const char *fmt, ...)
{
	char str[1000];

	va_list args;
	va_start(args, fmt);
	vsprintf(str, fmt, args);
	va_end(args);

	printf(str);
	#ifdef SP_WINDOWS
	OutputDebugStringA(str);
	#endif
}

static int64_t lizzz_timestamp()
{
	return (unsigned)time(NULL);

}

long long millis () {
	return time(NULL);
	/*
  struct timespec t ;
  clock_gettime ( CLOCK_REALTIME , & t ) ;
  return t.tv_sec * 1000 + ( t.tv_nsec + 500000 ) / 1000000 ;
  */
}

static int lizzz_sleep(int ms)
{
#ifdef SP_WINDOWS
	Sleep(ms);
#else
	usleep(ms * 1000);
#endif
	return 0;
}

int is_init = 0;
int init_ws32()
{
	if (is_init) return 1;
#ifdef SP_WINDOWS
	WSADATA wsaData;
	int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0) {
		printf("WSAStartup function failed with error: %d\n", iResult);
		MessageBoxA(NULL, "Error init ws32", "error", 0);
		exit(0);
	}
#endif
	is_init = 1;
	return 1;
}

void alert(std::string name, std::string message = "info")
{
#ifdef SP_WINDOWS
	MessageBoxA(NULL, name.c_str(), message.c_str(), 0);
#endif	
}

std::string HostToIp(const std::string& host) {
	init_ws32();
    hostent* hostname = gethostbyname(host.c_str());
    if(hostname)
        return std::string(inet_ntoa(**(in_addr**)hostname->h_addr_list));
    return {};
}

std::string to_string(int val)
{
	char buf[12];
	sprintf(buf, "%d", val);
	//itoa(val, buf, 10);
    
	std::string str = buf;
    return str;
}

const wchar_t* GetWC(const char* c)
{
	const size_t cSize = strlen(c) + 1;
	wchar_t* wc = new wchar_t[cSize];
	mbstowcs(wc, c, cSize);
	//MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, c, -1, wc, cSize);
	return wc;
}

void wcharToChar(std::wstring input, std::string& out) //Конфертация wchar_t* в char*
{
	int len = input.length();

	char* buff = (char*)malloc(len + 1);

	size_t n = wcstombs(buff, input.c_str(), len);
	buff[len] = '\0';

	out = buff;
	free(buff);

}

#ifdef SP_WINDOWS
void CharToWchar(std::string input, std::wstring &output) //Конфертация из char* (cp1251) to wchar_t*
{
	output = L"";
	const char *buff = input.c_str();
	int new_len = MultiByteToWideChar(1251, 0, buff, -1, NULL, 0); //CP_UTF8
	wchar_t* result = (wchar_t*)malloc((new_len + 1) * sizeof(wchar_t));
	MultiByteToWideChar(1251, 0, buff, -1, result, new_len);
	
	output.append(result, new_len);
	delete result;

}

void vs_output(const char* str)
{
	std::wstring out;
	CharToWchar(str, out);

	out = L"Debug: " + out;
	OutputDebugStringW(out.c_str());
}

void toUTF8(const wchar_t* src, std::string& out)
{
	if (!src)
	{
		return;
	}

	int src_length = wcslen(src);

	int length = WideCharToMultiByte(CP_UTF8, 0, src, src_length, 0, 0, NULL, NULL);
	char* output_buffer = (char*)malloc((length + 1) * sizeof(char));
	WideCharToMultiByte(CP_UTF8, 0, src, src_length, output_buffer, length, NULL, NULL);
	output_buffer[length] = '\0';

	out = "";
	out.append(output_buffer, length);

	free(output_buffer);

}

void convertToUTF8(std::string input, std::string &out)
{
	out = "";
	std::wstring fg;
	CharToWchar(input, fg);
	toUTF8(fg.c_str(), out);
}

void oemToString(std::string oem, std::string& out) //Для конвертации из строк полученных от винды в utf8/cp1251
{
	int len = oem.length();
	char* buff = (char*)malloc(len + 1);
	OemToCharA(oem.c_str(), buff);
	out = "";
	out.append(buff, len);
	free(buff);
	
}
#endif



