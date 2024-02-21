#include <string>

#include <fcntl.h>
#include <vector>

#ifdef SP_WINDOWS
#include <io.h>
#include <process.h>
#include <direct.h>
#endif

static const char* log_path = "C:\\log.txt";

struct FileObj
{
	std::string dir;
	std::string name;
};

class lizzz_env
{
public:
	static int init();

	static std::string getDir();
	static FileObj FileInfo();
	static int createDirRecursive(std::string path);
	static std::string getFileName();
	static int fileToBinArray(std::string file_name);

	static int findSymbolMirror(std::string& str, char split);
	static std::string getInstallDir(std::string name);
	
	static std::string getInfoSystem();
	static bool get_registry_value( uint32_t value_type, const std::string& registry_path, const std::string& registry_param, std::string& str_out, uint64_t& uint_out );

private:
	std::string current_dir;
};

#pragma once

#include "lizzz_log.h"
#include "lizzz_functions.h"


inline int lizzz_env::init()
{
	std::string roaming = getenv("APPDATA");
	std::string path = roaming + "\\log.txt";
	log_path = path.c_str();

	return 1;
}

inline std::string lizzz_env::getInstallDir(std::string name)
{
	std::string projectDir;
#ifdef SP_WINDOWS
	projectDir = getenv("APPDATA");
	projectDir += "\\" + name;
#else
	projectDir = "/tmp/" + name;
#endif
	
	createDirRecursive(projectDir + "\\");

	return projectDir;
}


inline int lizzz_env::fileToBinArray(std::string file_name) {
	std::string data = lizzz_filesystem::file_get_contents(file_name);
	std::string result = "{";
	for (int i = 0; i < data.length(); i++)
	{
		int c = data[i];
		char buff[11];
		sprintf(buff, "%d", c);

		result.append(buff, strlen(buff));
		result.append(", ", 2);

	}

	result += "};";

	lizzz_filesystem::file_put_contents(file_name + "_bin", result);
	return 1;
}




inline int lizzz_env::findSymbolMirror(std::string& str, char split) {
	//std::cout << str;
	for (int i = str.length(); i >= 0; i--) {
		if (str[i] == split) {
			return i;
		}
	}
	return -1;
}

inline int lizzz_env::createDirRecursive(std::string path)
{
	if (lizzz_filesystem::is_dir(path))
	{
		return 1;
	}
	//LOG("create recursive dir %s\r\n", path.c_str());
	std::string tmp = "";
	std::vector< std::string > v = lizzz_functions::explode(path, "\\");
	for (int i = 0; i < v.size() - 1; i++)
	{
		tmp += v[i] + "\\";
		int isDir = lizzz_filesystem::is_dir(tmp);
		//LOG("is_dir %s: %d\r\n", tmp.c_str(), isDir);
		if (!isDir)
		{
			lizzz_filesystem::create_dir(tmp.c_str());
			lizzz_Log::Instance()->addLog("Create dir: " + v[i] + " is_dir" + to_string(isDir));
		}

	}

	return 1;
}

inline FileObj lizzz_env::FileInfo()
{
	FileObj result;
	char ownPth[1024];

#ifdef SP_WINDOWS
	
	// Will contain exe path
	HMODULE hModule = GetModuleHandle(NULL);
	if (hModule == NULL)
	{
		//printf("Executable path NULL\r\n");
		return result;
	}

	// When passing NULL to GetModuleHandle, it returns handle of exe itself
	GetModuleFileNameA(hModule, ownPth, (sizeof(ownPth)));

	// Use above module handle to get the path using GetModuleFileName()



	std::string pathS = ownPth;


	std::string sDir;
	std::string sName;

	int pos = lizzz_env::findSymbolMirror(pathS, '\\');
	//printf("pos %d %s len %d\r\n", pos, str.c_str(), strlen(ownPth));
	if (pos != -1) {

		//OutputDebugStringA((pathS.substr(0, pos) + "\r\n").c_str());

		result.dir = pathS.substr(0, pos);
		result.name = pathS.substr(pos + 1, pathS.length() - pos - 1);
	}

#else

	ssize_t count = readlink("/proc/self/exe", ownPth, 260);
	std::string toRunable = std::string(ownPth, (count > 0) ? count : 0);

	int pos = lizzz_env::findSymbolMirror(toRunable, '/');
	//printf("pos %d %s\r\n", pos, toRunable.c_str());
	if (pos != -1) {
		result.dir = toRunable.substr(0, pos);
		result.name = toRunable.substr(pos + 1, toRunable.length() - pos - 1);
	}

	return result;
#endif

	return result;



}

inline std::string lizzz_env::getDir() {

	FileObj file = FileInfo();

	return file.dir;
}

inline std::string lizzz_env::getFileName() {

	FileObj file = FileInfo();
	return file.name;
}


inline std::string lizzz_env::getInfoSystem()
{
	
#ifdef SP_WINDOWS
	bool rez = false;
	uint64_t uint_out = 0;

	const std::string reg_path = "SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion";

	std::string product_name		= "ProductName";				// 'Windows 10 Pro'
	std::string edition_id			= "EditionID";					// 'Professional'
	std::string release_id			= "ReleaseId";					// '1909'
	std::string major_version		= "CurrentMajorVersionNumber";	// DWORD = 10
	std::string minor_version		= "CurrentMinorVersionNumber";	// DWORD = 0
	std::string current_build		= "CurrentBuild";				// '18363'
	std::string current_build_ex	= "UBR";						// DWORD = 1198
	//std::string product_id			= "ProductId";					// '00331-10000-00001-AA249'
	std::string current_version		= "CurrentVersion";				// 6.3 for win-10

	get_registry_value( uint32_t( REG_SZ ), reg_path, product_name, product_name, uint_out );
	get_registry_value( uint32_t( REG_SZ ), reg_path, edition_id, edition_id, uint_out );
	rez = get_registry_value( uint32_t( REG_SZ ), reg_path, release_id, release_id, uint_out );
	if ( !rez )
	{
		release_id = "_";
	}

	rez = get_registry_value( uint32_t( REG_DWORD ), reg_path, major_version, major_version, uint_out );
	rez &= get_registry_value( uint32_t( REG_DWORD ), reg_path, minor_version, minor_version, uint_out );
	if ( rez )
	{
		current_version = major_version + "." + minor_version;
	}
	else
	{
		get_registry_value( uint32_t( REG_SZ ), reg_path, current_version, current_version, uint_out );
	}

	get_registry_value( uint32_t( REG_SZ ), reg_path, current_build, current_build, uint_out );
	get_registry_value( uint32_t( REG_DWORD ), reg_path, current_build_ex, current_build_ex, uint_out );

	std::string result = product_name + " (" + edition_id + "), release: " + release_id + ", version: " + current_version + "." + current_build + "." + current_build_ex;
	return result;
	
#else
	std::string result = "Linux";
	return result;
#endif
}


inline bool lizzz_env::get_registry_value( uint32_t value_type, const std::string& registry_path, const std::string& registry_param, std::string& str_out, uint64_t& uint_out )
{
	

	uint_out = 0;
	bool read_rez = false;

#ifdef SP_WINDOWS

	char str_value[1000] = { 0 };
	uint64_t u64_value = 0;

	HKEY key = nullptr;
	DWORD v_type = DWORD( value_type );
	DWORD sz = 1000;
	
	int status = RegOpenKeyExA( HKEY_LOCAL_MACHINE, registry_path.c_str(), 0, KEY_READ, &key );
	if ( (status != ERROR_SUCCESS) || (!key) )
	{
		RegCloseKey( key );
		return false;
	}

	switch ( v_type )
	{
		case REG_SZ:				// Unicode nul terminated string
		case REG_EXPAND_SZ:			// Unicode nul terminated string (with environment variable references)
		{
			status = RegQueryValueExA( key, registry_param.c_str(), nullptr, &v_type, (LPBYTE)&str_value, &sz );
			if ( (status == ERROR_SUCCESS) && (sz != 0) )
			{
				str_out = str_value;
				read_rez = true;
			}
		}
		break;
		case REG_BINARY:			// Free form binary
		case REG_MULTI_SZ:			// Multiple Unicode strings
		case REG_RESOURCE_LIST:		// Resource list in the resource map
		case REG_LINK:				// Symbolic Link (unicode)
		case REG_FULL_RESOURCE_DESCRIPTOR:	// Resource list in the hardware description
		case REG_RESOURCE_REQUIREMENTS_LIST:
		{
			status = RegQueryValueExA( key, registry_param.c_str(), nullptr, &v_type, (LPBYTE)&str_value, &sz );
			if ( (status == ERROR_SUCCESS) && (sz != 0) )
			{
				str_out.resize( sz + 1, 0 );
				
				str_out = str_value;
				//memcpy( str_out.c_str(), str_value, sz );
				read_rez = true;
			}
		}
		break;
		case REG_DWORD:				// 32-bit number,  = REG_DWORD_LITTLE_ENDIAN
		case REG_DWORD_BIG_ENDIAN:	// 32-bit number
		case REG_QWORD:				// 64-bit number
		{
			status = RegQueryValueExA( key, registry_param.c_str(), nullptr, &v_type, (LPBYTE)&u64_value, &sz );
			if ( (status == ERROR_SUCCESS) && (sz != 0) )
			{
				uint_out = u64_value;
				str_out = std::to_string( u64_value );
				read_rez = true;
			}
		}
		break;
		case REG_NONE: // No value type
		default:
			read_rez = false;
			break;
	}

	RegCloseKey( key );

#endif

	return read_rez;
}