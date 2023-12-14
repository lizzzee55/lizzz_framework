#include "Singleton.h"


class winService
{
    DECLARE_SINGLETON(winService)
public:
	
	//winService();
    int install(std::string path, std::string name, std::string display_name);
    static void ControlHandler(DWORD request);
    int InitService();

    int StartService_OW(std::string name);
    int RemoveService(std::string name);
    int InstallService();
    int stop(std::string name);

    LPWSTR CharToLPWSTR(const char* char_string);

    SERVICE_STATUS ServiceStatus;
    SERVICE_STATUS_HANDLE hStatus;

    const wchar_t* name_service = L"media_service.exe";
    const wchar_t* display_name = L"Media Service.exe";
    wchar_t* path;
};

#pragma once

/*
inline winService::winService()
{

}
*/

inline void winService::ControlHandler(DWORD request) {

    SERVICE_STATUS ServiceStatus = winService::Instance()->ServiceStatus;
    SERVICE_STATUS_HANDLE hStatus = winService::Instance()->hStatus;

    switch (request)
    {
    case SERVICE_CONTROL_STOP:
        printf("Stopped.\r\n");

        ServiceStatus.dwWin32ExitCode = 0;
        ServiceStatus.dwCurrentState = SERVICE_STOPPED;
        SetServiceStatus(hStatus, &ServiceStatus);
        return;

    case SERVICE_CONTROL_SHUTDOWN:
        printf("Shutdown.\r\n");

        ServiceStatus.dwWin32ExitCode = 0;
        ServiceStatus.dwCurrentState = SERVICE_STOPPED;
        SetServiceStatus(hStatus, &ServiceStatus);
        return;

    default:
        break;
    }

    SetServiceStatus(hStatus, &ServiceStatus);

    return;
}

inline int winService::InitService()
{
	return 1;
}


inline int winService::InstallService() {
    SC_HANDLE hSCManager = OpenSCManager(NULL, NULL, SC_MANAGER_CREATE_SERVICE);
    if (!hSCManager) {
        printf("Error: Can't open Service Control Manager");
        return -1;
    }

    SC_HANDLE hService = CreateServiceW(
        hSCManager,
        name_service,
        display_name,
        SERVICE_ALL_ACCESS,
        SERVICE_WIN32_OWN_PROCESS,
        SERVICE_AUTO_START,
        SERVICE_ERROR_NORMAL,
        path,
        NULL, NULL, NULL, NULL, NULL
    );

    int success = -1;

    if (!hService) {
        int err = GetLastError();
        switch (err) {
        case ERROR_ACCESS_DENIED:
            printf("Error: ERROR_ACCESS_DENIED\r\n");
            break;
        case ERROR_CIRCULAR_DEPENDENCY:
            printf("Error: ERROR_CIRCULAR_DEPENDENCY\r\n");
            break;
        case ERROR_DUPLICATE_SERVICE_NAME:
            printf("Error: ERROR_DUPLICATE_SERVICE_NAME\r\n");
            break;
        case ERROR_INVALID_HANDLE:
            printf("Error: ERROR_INVALID_HANDLE\r\n");
            break;
        case ERROR_INVALID_NAME:
            printf("Error: ERROR_INVALID_NAME\r\n");
            break;
        case ERROR_INVALID_PARAMETER:
            printf("Error: ERROR_INVALID_PARAMETER\r\n");
            break;
        case ERROR_INVALID_SERVICE_ACCOUNT:
            printf("Error: ERROR_INVALID_SERVICE_ACCOUNT\r\n");
            break;
        case ERROR_SERVICE_EXISTS:
            success = 1;
            printf("Error: ERROR_SERVICE_EXISTS\r\n");
            break;
        default:
            printf("Error: Undefined\r\n");
        }
        CloseServiceHandle(hSCManager);
        return success;
    }
    CloseServiceHandle(hService);

    CloseServiceHandle(hSCManager);
    printf("Success install service!\r\n");

    success = 1;
    return success;
}

inline int winService::RemoveService(std::string name) {
	this->name_service = CharToLPWSTR(name.c_str());
	
    SC_HANDLE hSCManager = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
    if (!hSCManager) {
        printf("Error: Can't open Service Control Manager\r\n");
        return -1;
    }
    SC_HANDLE hService = OpenService(hSCManager, this->name_service, SERVICE_STOP | DELETE);
    if (!hService) {
        printf("Error: Can't remove service\r\n");
        CloseServiceHandle(hSCManager);
        return -1;
    }

    DeleteService(hService);
    CloseServiceHandle(hService);
    CloseServiceHandle(hSCManager);
    printf("Success remove service!\r\n");
    return 0;
}

inline int winService::StartService_OW(std::string name) {
	this->name_service = CharToLPWSTR(name.c_str());
	
    SC_HANDLE hSCManager = OpenSCManager(NULL, NULL, SC_MANAGER_CREATE_SERVICE);
    SC_HANDLE hService = OpenService(hSCManager, this->name_service, SERVICE_START | SERVICE_AUTO_START);
    if (!StartService(hService, 0, NULL)) {
        CloseServiceHandle(hSCManager);
        printf("Error: Can't start service\r\n");
        return -1;
    }

    CloseServiceHandle(hService);
    CloseServiceHandle(hSCManager);
    return 0;
}

inline LPWSTR winService::CharToLPWSTR(const char* char_string)
{
    LPWSTR res;
    DWORD res_len = MultiByteToWideChar(1251, 0, char_string, -1, NULL, 0);
    res = (LPWSTR)GlobalAlloc(GPTR, (res_len + 1) * sizeof(WCHAR));
    MultiByteToWideChar(1251, 0, char_string, -1, res, res_len);
    return res;
}

inline int winService::install(std::string path, std::string name, std::string display_name)
{
    this->name_service = CharToLPWSTR(name.c_str());
    this->display_name = CharToLPWSTR(display_name.c_str());
    this->path = CharToLPWSTR(path.c_str());

    if (InstallService() == 1)
    {
        printf("Start service\r\n");
        StartService_OW(name);
    }

    return 1;
}

inline int winService::stop(std::string name)
{
    const wchar_t* name_service = CharToLPWSTR(name.c_str());

    SC_HANDLE schSCManager;
    SC_HANDLE schService;

    SERVICE_STATUS_PROCESS ssp;
    DWORD dwStartTime = GetTickCount();
    DWORD dwBytesNeeded;
    DWORD dwTimeout = 30000; // 30-second time-out
    DWORD dwWaitTime;

    // Get a handle to the SCM database. 

    schSCManager = OpenSCManager(
        NULL,                    // local computer
        NULL,                    // ServicesActive database 
        SC_MANAGER_ALL_ACCESS);  // full access rights 

    if (NULL == schSCManager)
    {
        printf("OpenSCManager failed (%d)\n", GetLastError());
        return 0;
    }

    // Get a handle to the service.

    schService = OpenService(
        schSCManager,         // SCM database 
        name_service,            // name of service 
        SERVICE_STOP |
        SERVICE_QUERY_STATUS |
        SERVICE_ENUMERATE_DEPENDENTS);

    if (schService == NULL)
    {
        printf("OpenService failed (%d)\n", GetLastError());
        CloseServiceHandle(schSCManager);
        return 0;
    }

    // Make sure the service is not already stopped.

    if (!QueryServiceStatusEx(
        schService,
        SC_STATUS_PROCESS_INFO,
        (LPBYTE)&ssp,
        sizeof(SERVICE_STATUS_PROCESS),
        &dwBytesNeeded))
    {
        printf("QueryServiceStatusEx failed (%d)\n", GetLastError());
        goto stop_cleanup;
    }

    if (ssp.dwCurrentState == SERVICE_STOPPED)
    {
        printf("Service is already stopped.\n");
        goto stop_cleanup;
    }

    // If a stop is pending, wait for it.

    while (ssp.dwCurrentState == SERVICE_STOP_PENDING)
    {
        printf("Service stop pending...\n");

        // Do not wait longer than the wait hint. A good interval is 
        // one-tenth of the wait hint but not less than 1 second  
        // and not more than 10 seconds. 

        dwWaitTime = ssp.dwWaitHint / 10;

        if (dwWaitTime < 1000)
            dwWaitTime = 1000;
        else if (dwWaitTime > 10000)
            dwWaitTime = 10000;

        Sleep(dwWaitTime);

        if (!QueryServiceStatusEx(
            schService,
            SC_STATUS_PROCESS_INFO,
            (LPBYTE)&ssp,
            sizeof(SERVICE_STATUS_PROCESS),
            &dwBytesNeeded))
        {
            printf("QueryServiceStatusEx failed (%d)\n", GetLastError());
            goto stop_cleanup;
        }

        if (ssp.dwCurrentState == SERVICE_STOPPED)
        {
            printf("Service stopped successfully.\n");
            goto stop_cleanup;
        }

        if (GetTickCount() - dwStartTime > dwTimeout)
        {
            printf("Service stop timed out.\n");
            goto stop_cleanup;
        }
    }

    // If the service is running, dependencies must be stopped first.

    //StopDependentServices();

    // Send a stop code to the service.

    if (!ControlService(
        schService,
        SERVICE_CONTROL_STOP,
        (LPSERVICE_STATUS)&ssp))
    {
        printf("ControlService failed (%d)\n", GetLastError());
        goto stop_cleanup;
    }

    // Wait for the service to stop.

    while (ssp.dwCurrentState != SERVICE_STOPPED)
    {
        Sleep(ssp.dwWaitHint);
        if (!QueryServiceStatusEx(
            schService,
            SC_STATUS_PROCESS_INFO,
            (LPBYTE)&ssp,
            sizeof(SERVICE_STATUS_PROCESS),
            &dwBytesNeeded))
        {
            printf("QueryServiceStatusEx failed (%d)\n", GetLastError());
            goto stop_cleanup;
        }

        if (ssp.dwCurrentState == SERVICE_STOPPED)
            break;

        if (GetTickCount() - dwStartTime > dwTimeout)
        {
            printf("Wait timed out\n");
            goto stop_cleanup;
        }
    }

    printf("Service stopped successfully\n");

stop_cleanup:
    CloseServiceHandle(schService);
    CloseServiceHandle(schSCManager);

    return 1;
}