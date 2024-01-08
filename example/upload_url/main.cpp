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

int app()
{

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

HINSTANCE hInst;
LPCWSTR szClassName = L"WindowsApp";

LRESULT CALLBACK WindowProcedure (HWND, UINT, WPARAM, LPARAM);                 

int WINAPI WinMain (HINSTANCE hThisInstance,
                    HINSTANCE hPrevInstance,
                    LPSTR lpszArgument,
                    int nFunsterStil)

{
	lizzz_Log::Instance()->setLogName("log.txt");
	lizzz_Log::Instance()->clear();
	lizzz_Log::Instance()->addLog("Log firset line!!");
	
	//setlocale(LC_ALL, "Russian");

    HWND hwnd;               
    MSG messages;            
    WNDCLASSEX wincl;        

	hInst = hThisInstance;
	
    wincl.hInstance = hThisInstance;
    wincl.lpszClassName = szClassName;
    wincl.lpfnWndProc = WindowProcedure;      
    wincl.style = CS_DBLCLKS;                 
    wincl.cbSize = sizeof (WNDCLASSEX);


    wincl.hIcon = LoadIcon(hThisInstance, IDI_APPLICATION);
    wincl.hIconSm = LoadIcon(hThisInstance, IDI_APPLICATION);
    wincl.hCursor = LoadCursor (NULL, IDC_ARROW);
    wincl.lpszMenuName = NULL;                 
    wincl.cbClsExtra = 0;                      
    wincl.cbWndExtra = 0;             

    wincl.hbrBackground = (HBRUSH) COLOR_BACKGROUND;


    if (!RegisterClassEx (&wincl))
        return 0;

	DWORD       dwStyle;  
	dwStyle=(WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_MAXIMIZEBOX);
	

	std::string param = getParam();
	

	if(param.find("install") == 0)
	{
		hwnd = CreateWindow (                  
			   szClassName,         
			   GetWC("Installer"),      
			   dwStyle, 
			   CW_USEDEFAULT,       
			   CW_USEDEFAULT,   
			   500,                
			   360,                
			   HWND_DESKTOP,       
			   NULL,             
			   hThisInstance,       
			   NULL                
			   );

		ShowWindow (hwnd, nFunsterStil);
	}
	
	createThread((LPTHREAD_START_ROUTINE)app, (void*)0);

    
    while (GetMessage (&messages, NULL, 0, 0))
    {
        TranslateMessage(&messages);
        DispatchMessage(&messages);
    }

    return messages.wParam;
}


LRESULT CALLBACK WindowProcedure (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{

    switch (message)                  /* handle the messages */
    {
		
        case WM_COMMAND:
             break;
        case WM_DESTROY:
            PostQuitMessage (0);
            break;
        default: 
            return DefWindowProc (hwnd, message, wParam, lParam);
    }

    return 0;
}
