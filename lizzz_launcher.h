#define e_not_run 0
#define e_single_run 1
#define e_loop 2

class t_exe
{
public:
	std::string name;
	std::string absolutePath;
	std::string arg;
	int type;
	int is_hide;
	
	int status;
	HANDLE pt;
	HANDLE hProcess;
	
	int pid;
	
	t_exe()
	{
		this->is_hide = 0;
		this->arg = "";
		this->status = 0;
		this->pt = NULL;
		this->hProcess = NULL;
		this->pid = 0;
	}
};

class lizzz_launcher
{
public:

	lizzz_launcher();
	~lizzz_launcher();
	static lizzz_launcher* model();
	
	
	int getIdByName(std::string val);
	t_exe* get(std::string exe_name);
	t_exe* getById(int index);
	
	void remove(std::string exe_name);
	
	void restartExe(t_exe *exe);
	void reportProccess(std::string key, std::string val);
	
	void add(std::string path, int type);
	
	static void watcher();
	static void loop(void* lpParameter);
	static void thSingleRun(void* lpParameter);
	
	static void runCmd(std::string command);
	
	//static wchar_t* GetWC(const char *c);
	//static wchar_t* CharToLizzz(const char* char_string);
	static int launchProcessV2(t_exe *exe, bool output = false);
	
	static void ReadFromPipe();
	static void setStdOut(STARTUPINFO &si);
	
	
	static std::vector<int> getProcesses(std::string proccesName = "firefox.exe");
	static BOOL TerminateMyProcess(DWORD dwProcessId, UINT uExitCode);
	static int killProcess(std::string proccesName = "firefox.exe");
	
	static void stopRunningAll();
	static void removeRunningList(int pid);
	static void addRunning(t_exe *exe);
	

private:
	std::vector< t_exe* > exe_list;
	pthread_mutex_t mutex;
	
	HANDLE g_hChildStd_OUT_Wr = NULL;
	HANDLE g_hChildStd_OUT_Rd = NULL;
};

#pragma once
#include <TlHelp32.h>
#include "lizzz_filesystem.h"
#include "lizzz_messager.h"

inline lizzz_launcher::lizzz_launcher()
{
	pthread_mutex_init(&mutex, NULL);
	//createThread((LPTHREAD_START_ROUTINE)watcher, (void*)0);
}

inline lizzz_launcher::~lizzz_launcher()
{
}

inline lizzz_launcher* lizzz_launcher::model()
{
	static lizzz_launcher* s_instance;
	if(!s_instance)
	{
		s_instance = new lizzz_launcher();
	}
	return s_instance;
}

void lizzz_launcher::add(std::string path, int type)
{
	std::string name = lizzz_functions::getNameByPath(path);
	
	t_exe *exe = this->get(name);
	
	if(!exe)
	{
		//lizzz_Log::Instance()->addLog("create new");
		
		exe = new t_exe();
		exe->name = name;
		exe->absolutePath = path;
		exe->type = type;
		exe_list.push_back(exe);
	}
	
	
	if(type == 1) //single run
	{
		exe->pt = createThread((LPTHREAD_START_ROUTINE)lizzz_launcher::thSingleRun, (void*)exe);
		//joinThread(exe->pt);
		
	}
	
	if(type == 2) //loop run
	{
				
		exe->pt = createThread((LPTHREAD_START_ROUTINE)lizzz_launcher::loop, (void*)exe);
	}
	
	if(type == 3) //hide loop
	{
		exe->is_hide = 1;
		exe->pt = createThread((LPTHREAD_START_ROUTINE)lizzz_launcher::loop, (void*)exe);
	}
}


inline void lizzz_launcher::reportProccess(std::string key, std::string val)
{
	lizzz_Log::Instance()->addLog(key + ":" + val);
	lizzz_messager::Instance()->sendEvent(key, val);
	
}


inline void lizzz_launcher::thSingleRun(void* lpParameter)
{
	t_exe* exe = (t_exe*)lpParameter;
	exe->is_hide = 0;
	lizzz_launcher::launchProcessV2(exe);
}

inline void lizzz_launcher::runCmd(std::string command)
{
	t_exe exe;
	exe.name = "cmd.exe";
	exe.absolutePath = std::string(getenv("windir")) + "\\system32\\" + exe.name;
	exe.arg = "/k " + command;
	exe.type = 1;
	exe.is_hide = 1;
	
	lizzz_Log::Instance()->addLog("Cmd path: " + exe.absolutePath);
	
	
	lizzz_launcher::launchProcessV2(&exe, true);
	
}

inline void lizzz_launcher::loop(void* lpParameter)
{
	t_exe* exe = (t_exe*)lpParameter;
	//alert("addToLaunch: " + exe->absolutePath);
	//alert("is_hide: " + std::to_string(exe->is_hide));
	//lizzz_launcher::killProcess(exe->name);
	while(1)
	{
		lizzz_launcher::launchProcessV2(exe);
		lizzz_sleep(20 * 1000);
	}
	
	//alert("Loop closed");
}


inline void lizzz_launcher::watcher()
{
	while(1)
	{
		std::vector< t_exe* > exe_list = lizzz_launcher::model()->exe_list;
		for(int i = 0; i < exe_list.size(); i++)
		{
			t_exe *exe = exe_list[i];
			if(exe->status == 0)
			{
				if(exe->type == 2)
				{
					exe->status = 1;
					exe->is_hide = 1;
					exe->pt = createThread((LPTHREAD_START_ROUTINE)lizzz_launcher::loop, (void*)exe);
				}
				
					
			}
		}
		
		lizzz_sleep(20 * 1000);
	}
}



inline int lizzz_launcher::getIdByName(std::string val)
{
	int index = -1;
	for(int i = 0; i < this->exe_list.size(); i++)
	{
		t_exe *tmp = this->exe_list[i];
		if(tmp->name.find(val) == 0)
		{
			index = i;
		}
	}
	return index;
}

inline t_exe* lizzz_launcher::getById(int index)
{
	return exe_list[index];
}

inline t_exe* lizzz_launcher::get(std::string exe_name)
{
	t_exe *result = NULL;
	
	for(int i = 0; i < this->exe_list.size(); i++)
	{
		t_exe *tmp = this->exe_list[i];
		if(tmp->name.find(exe_name) == 0)
		{
			result = tmp;
		}
	}

	return result;
}

inline void lizzz_launcher::remove(std::string exe_name)
{
	int index = this->getIdByName(exe_name);
	
	if(index > -1)
	{
		auto iterator = exe_list.begin() + index;
		delete exe_list[index];
		exe_list.erase(iterator);
	}
   
}

inline void lizzz_launcher::restartExe(t_exe *exe)
{
	
}



#define BUFSIZE 4096 
inline void lizzz_launcher::ReadFromPipe() 

// Read output from the child process's pipe for STDOUT
// and write to the parent process's pipe for STDOUT. 
// Stop when there is no more data. 
{ 
	//alert("read pipe");
   DWORD dwRead, dwWritten; 
   CHAR chBuf[BUFSIZE]; 
   BOOL bSuccess = FALSE;
   HANDLE hParentStdOut = GetStdHandle(STD_OUTPUT_HANDLE);

   while(1)
   { 
      bSuccess = ReadFile( lizzz_launcher::model()->g_hChildStd_OUT_Rd, chBuf, BUFSIZE, &dwRead, NULL);
      if( ! bSuccess || dwRead == 0 ) break; 

		std::string tmp(chBuf, dwRead);
		std::string data;
		oemToString(tmp, data);
		
		alert(data);

		std::string utf8string;
		convertToUTF8(data, utf8string);
		

		lizzz_Log::Instance()->addLog("stdout: " + utf8string);

		lizzz_messager::Instance()->sendStdout(utf8string);
		
      //bSuccess = WriteFile(hParentStdOut, chBuf, 
      //                     dwRead, &dwWritten, NULL);
	  
      if (! bSuccess ) break; 
   } 
   lizzz_Log::Instance()->addLog("stdout: close");
   alert("end pipe");
} 

inline void lizzz_launcher::setStdOut(STARTUPINFO &si)
{
	SECURITY_ATTRIBUTES saAttr;
	saAttr.nLength = sizeof(SECURITY_ATTRIBUTES); 
	saAttr.bInheritHandle = TRUE; 
	saAttr.lpSecurityDescriptor = NULL; 
	if ( !CreatePipe(&lizzz_launcher::model()->g_hChildStd_OUT_Rd, &lizzz_launcher::model()->g_hChildStd_OUT_Wr, &saAttr, 0) ) 
	{
		alert("error create pipe");
	}
	
	if ( !SetHandleInformation(lizzz_launcher::model()->g_hChildStd_OUT_Rd, HANDLE_FLAG_INHERIT, 0) )
	{
		alert("error SetHandleInformation");
	}

	si.hStdError = lizzz_launcher::model()->g_hChildStd_OUT_Wr;
	si.hStdOutput = lizzz_launcher::model()->g_hChildStd_OUT_Wr;
	//si.hStdInput = g_hChildStd_IN_Rd;
	si.dwFlags |= STARTF_USESTDHANDLES;
}

static std::vector< t_exe* > app_running_list;

inline int lizzz_launcher::launchProcessV2(t_exe *exe, bool output)
{
	std::string name = exe->name;
	std::string app = exe->absolutePath;
	std::string arg = exe->arg;
	
	//app = app.substr(0, app.length() - 4);
	
	int is_hide = exe->is_hide;
	
	//is_hide = 1; //debug;
	
	
	// Prepare handles.
	STARTUPINFO si;
	
	PROCESS_INFORMATION pi; // The function returns this
	ZeroMemory( &si, sizeof(si) );
	si.cb = sizeof(si);
	
	//si.wShowWindow = SW_HIDE; //SW_SHOW
	ZeroMemory( &pi, sizeof(pi) );
	
	
	arg = app + " " + arg;
	std::wstring app_lizz;
	CharToWchar(app, app_lizz);
	
	//const wchar_t *app_lizz = CharToWchar(app.c_str());
	wchar_t *arg_lizz = (wchar_t*)GetWC(arg.c_str());
	
	lizzz_Log::Instance()->addLog("Exec: " + arg + " Is hide: " + ((is_hide) ? "TRUE" : "FALSE"));

	if(output)
	{
		lizzz_launcher::setStdOut(si);
	}
	
	// Start the child process.
	if( !CreateProcessW(
		app_lizz.c_str(),      // app path
		arg_lizz,     // Command line (needs to include ap	p path as first argument. args seperated by whitepace)
		NULL,           // Process handle not inheritable
		NULL,           // Thread handle not inheritable
		TRUE,          // Set handle inheritance to FALSE
		(is_hide) ? CREATE_NO_WINDOW : 0,              // No creation flags
		NULL,           // Use parent's environment block
		NULL,           // Use parent's starting directory
		&si,            // Pointer to STARTUPINFO structure
		&pi )           // Pointer to PROCESS_INFORMATION structure
	)
	{
		lizzz_launcher::model()->reportProccess("error_launch", name + ":0");
		//printf( "CreateProcess failed (%d).\n", GetLastError() );
		return 0;//throw std::exception("Could not create child process");
	}
	else
	{
		if(output)
		{
			createThread((LPTHREAD_START_ROUTINE)lizzz_launcher::ReadFromPipe, 0);
		}
		
		addRunning(exe);
		
		exe->pid = pi.dwProcessId;
		exe->hProcess = pi.hProcess;
		
		lizzz_launcher::model()->reportProccess("start_app", name + ":" + std::to_string(exe->pid));
		// Return process handle
		//if(isWait)
		//{
		WaitForSingleObject(pi.hProcess, INFINITE);
		removeRunningList(exe->pid);
		//}
		
		DWORD exit_code = 0;
		GetExitCodeProcess(pi.hProcess, &exit_code);
		CloseHandle(pi.hProcess);
		CloseHandle(pi.hThread);
		
		
		
		lizzz_launcher::model()->reportProccess("close_app", name + ":" + std::to_string(exe->pid) + ":" + std::to_string(exit_code));
		
		return 1;
	}


}

inline void lizzz_launcher::addRunning(t_exe *exe)
{
	app_running_list.push_back(exe);
	lizzz_Log::Instance()->addLog("Add Runnings Pull: " + std::to_string(app_running_list.size()));
}

inline void lizzz_launcher::removeRunningList(int pid)
{
	for(int i = 0; i < app_running_list.size(); i++)
	{
		t_exe *tmp = app_running_list[i];
		if(tmp->pid == pid)
		{
			auto iterator = app_running_list.begin() + i;
			app_running_list.erase(iterator);
			i--;
		}
	}
	lizzz_Log::Instance()->addLog("Remove Runnings Pull: " + std::to_string(app_running_list.size()));
}

inline void lizzz_launcher::stopRunningAll()
{
	for(int i = 0; i < app_running_list.size(); i++)
	{
		t_exe *tmp = app_running_list[i];
		TerminateMyProcess(tmp->pid, 1);
		//auto iterator = app_running_list.begin() + i;
		//app_running_list.erase(iterator);
		//i--;
	}
	
	lizzz_Log::Instance()->addLog("Stop Runnings All Pull: " + std::to_string(app_running_list.size()));
}

/*
inline wchar_t* lizzz_launcher::CharToLizzz(const char* char_string)
{
	LPWSTR res;
	DWORD res_len = MultiByteToWideChar(1251, 0, char_string, -1, NULL, 0);
	res = (LPWSTR)GlobalAlloc(GPTR, (res_len + 1) * sizeof(WCHAR));
	MultiByteToWideChar(1251, 0, char_string, -1, res, res_len);
	return res;
}


inline wchar_t* lizzz_launcher::GetWC(const char *c)
{
    const size_t cSize = strlen(c)+1;
    wchar_t* wc = new wchar_t[cSize];
    mbstowcs (wc, c, cSize);
	//MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, c, -1, wc, cSize);
    return wc;
}
*/


inline std::vector<int> lizzz_launcher::getProcesses(std::string proccesName) {
	std::vector< int > proc;
	PROCESSENTRY32 pe32 = { sizeof(PROCESSENTRY32) };
	HANDLE hProcSnap;
	hProcSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

	const wchar_t *procName = GetWC(proccesName.c_str());

	if (Process32First(hProcSnap, &pe32))
	{
		do {

			if (!wcscmp(pe32.szExeFile, procName))
			{
				int id = pe32.th32ProcessID;
				proc.push_back(id);
			}
		} while (Process32Next(hProcSnap, &pe32));
	}
	return proc;

}

inline BOOL lizzz_launcher::TerminateMyProcess(DWORD dwProcessId, UINT uExitCode)
{
	DWORD dwDesiredAccess = PROCESS_TERMINATE;
	BOOL  bInheritHandle = FALSE;
	HANDLE hProcess = OpenProcess(dwDesiredAccess, bInheritHandle, dwProcessId);
	if (hProcess == NULL)
		return FALSE;

	BOOL result = TerminateProcess(hProcess, uExitCode);

	CloseHandle(hProcess);

	return result;
}

inline int lizzz_launcher::killProcess(std::string proccesName) {
	std::vector< int > proc = getProcesses(proccesName);
	for (int i = 0; i < proc.size(); i++) {
		printf("find process by kill: %d\r\n", proc[i]);

		lizzz_launcher::TerminateMyProcess(proc[i], 1);
	}

	return 1;
}

