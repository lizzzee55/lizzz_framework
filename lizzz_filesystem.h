#include <string>

class lizzz_filesystem
{
public:
	static std::string file_get_contents(std::string fileName);
	static int file_put_contents(std::string file_name, std::string bin, bool append = 0);
	static int copyFile(std::string source, std::string dest);
	static int is_dir(std::string dir);
	
	static wchar_t* GetWC(const char *c);
	static wchar_t* CharToLizzz(const char* char_string);
	static int launchProcess(std::string app, std::string arg = "", int isHide = 0, bool isWait = false);
	
	static int exist_file(std::string file_name);
	static int create_dir(std::string path);
	
	static int listDir(std::vector< std::string > &out, std::string path);
};

#pragma once

#ifndef WIN32
#include <dirent.h>
#include <errno.h>
#include <spawn.h>
#include <sys/wait.h>
#include <unistd.h>
#define O_BINARY 0x8000
#else

#endif


#include <fcntl.h>
#include <sys/stat.h>
#include "lizzz_log.h"

inline int lizzz_filesystem::exist_file(std::string file_name)
{
	int fd = open(file_name.c_str(), O_RDONLY);
	if (fd > 0)
	{
		close(fd);
		return 1;
	}
	return 0;
}


inline int lizzz_filesystem::listDir(std::vector< std::string > &out, std::string path)
{
	out.clear();
	
	lizzz_Log::Instance()->addLog("log_service.txt", "List dir: " + path);
	int count = 0;
#ifdef WIN32

	path = path + "/*.*";
	WIN32_FIND_DATA fd;
	HANDLE hFind = ::FindFirstFile(CharToLizzz(path.c_str()), &fd);
	if (hFind != INVALID_HANDLE_VALUE) {
		do {
			// read all (real) files in current folder
			// , delete '!' read other 2 default folder . and ..
			//if ((fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
				std::wstring input;
				input = fd.cFileName;

				std::string output;
				toUTF8(input.c_str(), output);

				out.push_back(output);
			//}
		} while (::FindNextFile(hFind, &fd));
		::FindClose(hFind);
	}

	/*
	DIR *dir;
	struct dirent *ent;
	if ((dir = opendir (path.c_str())) != NULL) 
	{
		while ((ent = readdir (dir)) != NULL) 
		{
			
			char tmp[1024];
			sprintf (tmp, "%s\n", ent->d_name);
			if(tmp[0] == '.') continue;
			
			out.push_back(tmp);
			//alert(tmp);
		}
		closedir (dir);
		count = out.size();
	} else {
		//could not open directory 
	}
	*/
#endif

	return count;
}

inline int lizzz_filesystem::is_dir(std::string dir)
{
#ifdef WIN32
	DWORD dwAttrib = GetFileAttributesA(dir.c_str());

	return (dwAttrib != INVALID_FILE_ATTRIBUTES &&
		(dwAttrib & FILE_ATTRIBUTE_DIRECTORY));
#endif
	return 0;
}

inline int lizzz_filesystem::create_dir(std::string path)
{
#ifdef WIN32
	mkdir(path.c_str());
#else
	mkdir(path.c_str(), 0777);
#endif
	return 1;
}

/*
static bool copyFile(const char *SRC, const char* DEST)
	{

		std::ifstream src(SRC, std::ios::binary);
		std::ofstream dest(DEST, std::ios::binary);
		dest << src.rdbuf();
		return src && dest;
	}
*/

inline int lizzz_filesystem::copyFile(std::string source, std::string dest)
{
	std::string data = lizzz_filesystem::file_get_contents(source);
	lizzz_filesystem::file_put_contents(dest, data);
	return 1;
}

inline std::string lizzz_filesystem::file_get_contents(std::string file_name) {
	std::string data;

	int fd = -1;

	fd = open(file_name.c_str(), O_RDONLY | O_BINARY);

	if (fd > 0)
	{
		int len = 0;
		char buffer[1024];

		while ((len = read(fd, buffer, sizeof buffer)) > 0)
		{
			data.append(buffer, len);
		}

		close(fd);
	}

	return data;
}

inline int lizzz_filesystem::file_put_contents(std::string file_name, std::string bin, bool append) {
	
	
	int fd = -1;
	if (append)
	{
		fd = open(file_name.c_str(), O_WRONLY | O_BINARY | O_CREAT | O_APPEND, S_IWRITE | S_IREAD);
	}
	else {
		fd = open(file_name.c_str(), O_WRONLY | O_BINARY | O_CREAT | O_TRUNC, S_IWRITE); //O_RDWR
	}

	if (fd <= 0)
	{
		return -1;
	}
		
	int w_len = write(fd, bin.data(), bin.length());

	close(fd);

	return w_len;

}

inline int lizzz_filesystem::launchProcess(std::string app, std::string arg, int isHide, bool isWait)
{

#ifdef WIN32
	// Prepare handles.
	STARTUPINFO si;
	
	PROCESS_INFORMATION pi; // The function returns this
	ZeroMemory( &si, sizeof(si) );
	si.cb = sizeof(si);
	
	//si.wShowWindow = SW_HIDE; //SW_SHOW
	ZeroMemory( &pi, sizeof(pi) );
	
	
	arg = app + " " + arg;
	//const wchar_t *app_lizz = lizzz_filesystem::CharToLizzz(app.c_str());
	
	std::wstring app_lizz;
	CharToWchar(app, app_lizz);
	wchar_t *arg_lizz = lizzz_filesystem::GetWC(arg.c_str());
	
	lizzz_Log::Instance()->addLog("log_service.txt", "Exec: " + arg);

	
	// Start the child process.
	if( !CreateProcessW(
		app_lizz.c_str(),      // app path
		arg_lizz,     // Command line (needs to include app path as first argument. args seperated by whitepace)
		NULL,           // Process handle not inheritable
		NULL,           // Thread handle not inheritable
		FALSE,          // Set handle inheritance to FALSE
		(isHide) ? CREATE_NO_WINDOW : 0,              // No creation flags
		NULL,           // Use parent's environment block
		NULL,           // Use parent's starting directory
		&si,            // Pointer to STARTUPINFO structure
		&pi )           // Pointer to PROCESS_INFORMATION structure
	)
	{
		//printf( "CreateProcess failed (%d).\n", GetLastError() );
		return 0;//throw std::exception("Could not create child process");
	}
	else
	{
		int pid = pi.dwProcessId;
		// Return process handle
		if(isWait)
		{
			WaitForSingleObject(pi.hProcess, INFINITE);
			
		}
		
		DWORD exit_code = 0;
		GetExitCodeProcess(pi.hProcess, &exit_code);
		CloseHandle(pi.hProcess);
		CloseHandle(pi.hThread);
		return (int)exit_code + 1;
	}
	
#else
	
	std::string full_command_line = app + " " + arg;

	char *argv[2];
	argv[0] = (char*)app.c_str();
	argv[1] = (char*)arg.c_str();
	argv[2] = 0;

	
	sigset_t sigset;

	sigemptyset(&sigset);
  	//sigaddset(&sigset, SIGTSTP); //блокируем только ctrl+z
	//sigfillset(&mask); Блокировать все сигналы

	posix_spawnattr_t attr;
	posix_spawnattr_init(&attr);
	posix_spawnattr_setflags(&attr, POSIX_SPAWN_SETSIGMASK);
	posix_spawnattr_setsigmask(&attr, &sigset);

	posix_spawn_file_actions_t action;
    posix_spawn_file_actions_init(&action);
    //posix_spawn_file_actions_addopen (&action, STDOUT_FILENO, "/dev/null", O_WRONLY|O_APPEND, 0);

	//char *environ = "";

	int pid = -1;
	int status = -1;
	//status = system(full_command_line.c_str()); //"g++ file.cpp -o executable"
	//pid = getpid();
	//if(status == -1)
	status = posix_spawn(&pid, app.c_str(), &action, &attr, argv, environ); //int code = system(full_command_line.c_str());
	if(status != 0)
	{
		printf("Error create spawn %d\r\n", status);
		return 0;
	}
	
	int statusWait = -1;
	int rez = waitpid(pid, &statusWait, 0);

	printf("Wait %d", pid);
	if (rez == -1) {
		return 0;
	}
	
	int exit_code = statusWait;
	if ( WIFEXITED(status) )
	{
		exit_code = WEXITSTATUS(statusWait);
	}
	
	return exit_code;
#endif

	return 0;
}

inline wchar_t* lizzz_filesystem::CharToLizzz(const char* char_string)
{
	wchar_t* res;
#ifdef WIN32
	
	DWORD res_len = MultiByteToWideChar(1251, 0, char_string, -1, NULL, 0);
	res = (wchar_t*)GlobalAlloc(GPTR, (res_len + 1) * sizeof(WCHAR));
	MultiByteToWideChar(1251, 0, char_string, -1, res, res_len);
#else
	
	//static wchar_t* res;
	//if(res) delete res;
	
	//int newLen = strlen(char_string) + 1;
	//res = new wchar_t[newLen];
	
#endif

	return res;
}


inline wchar_t* lizzz_filesystem::GetWC(const char *c)
{
    const size_t cSize = strlen(c)+1;
    wchar_t* wc = new wchar_t[cSize];
    mbstowcs (wc, c, cSize);
	//MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, c, -1, wc, cSize);
    return wc;
}