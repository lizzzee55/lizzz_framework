#include <vector>
#include <string>
#include <algorithm>

class lizzz_functions
{
public:
	
	static std::vector<std::string> explode(std::string str, const char *delim);
	static std::string implode(std::vector<std::string> arr, const char *delim);
	static std::string w_char_to_string(wchar_t *str);
	static const wchar_t* string_to_wchar(std::string str);
	static std::vector< char* > explode_fast(std::string str, const char *delim);
	
	//static int lizzz_explode(std::vector< std::string > &output, std::string str, const char *delim);
	static int explode_twice(std::vector< std::string > &output, std::string str, std::string delim);
	
	static std::string getNameByPath(std::string path);
	
	static int lizzz_explode(std::vector< std::string > &output, std::string str, std::string delim);
	
	static int asciitolower(char in) {
		if (in <= 'Z' && in >= 'A')
			return in - ('Z' - 'z');
		return in;
	}

	static void ft_tolower(std::string &str)
	{
		std::transform(str.begin(), str.end(), str.begin(), asciitolower);
	}

	static void lt(std::string& str)
	{
		int i = 0;
		while (str[i] == ' ' || str[i] == '\n' || str[i] == '\r' || str[i] == '\t') { i++; }

		//printf("i = %d %s\r\n", i, str.c_str());

		char* buff = (char*)malloc(str.length() - i + 1);
		
		int n = 0;
		for (int l = i; l < str.length(); l++)
		{
			buff[n++] = str[l];
		}
		buff[n] = 0;

		str = buff;

		free( buff );
	}

	static void rt(std::string& str)
	{
		if (str.length() <= 0) return;

		int i = str.length() - 1;
		while (str[i] == 32 || str[i] == 10 || str[i] == 13 || str[i] == 9) { i--; }

		i++;

		char* buff = (char*)malloc(i + 1);

		int n = 0;
		for (int l = 0; l < i; l++)
		{
			buff[n++] = str[l];
		}
		buff[n] = 0;

		str = buff;

		free (buff);
	}

	/*
	static inline std::string &ltrim(std::string &s) {
		s.erase(s.begin(), std::find_if(s.begin(), s.end(),
			std::not1(std::ptr_fun<int, int>(std::isspace))));
		return s;
	}

	// trim from end
	static inline std::string &rtrim(std::string &s) {
		s.erase(std::find_if(s.rbegin(), s.rend(),
			std::not1(std::ptr_fun<int, int>(std::isspace))).base(), s.end());
		return s;
	}
	*/

	// trim from both ends
	static inline std::string &trim(std::string &s) {
		lt(s);
		rt(s);
		return s;// ltrim(rtrim(s));
	}
};

#pragma once

//#include "lizzz_exeption.h"
#include "lizzz_log.h"

inline std::string lizzz_functions::implode(std::vector<std::string> arr, const char *delim)
{
	std::string result;
	for (int i = 0; i < arr.size(); i++)
	{
		result += arr[i] + delim;
	}

	result = result.substr(0, result.length() - strlen(delim));

	return result;
}

inline std::string lizzz_functions::getNameByPath(std::string path)
{
	std::vector< std::string > tmp = lizzz_functions::explode(path, "/");
	return tmp[tmp.size() - 1];
}

inline std::string lizzz_functions::w_char_to_string(wchar_t *str)
{
	std::wstring ws(str);
	// your new String
	std::string str_res(ws.begin(), ws.end());

	return str_res;
}

inline const wchar_t* lizzz_functions::string_to_wchar(std::string str)
{

	const size_t cSize = str.length() + 1;
	wchar_t* wc = new wchar_t[cSize];
	mbstowcs(wc, str.c_str(), cSize);

	return wc;

}

static int find_pos(const char* buf, const char* delim)
{
	int buf_len = strlen(buf);
	int delim_len = strlen(delim);

	for (int i = 0; i < buf_len; i++)
	{
		int j;
		for (j = 0; j < delim_len; j++)
		{
			if (buf[i + j] != delim[j])
				break;
		}

		//Log("log.txt", std::string("ln ") + _itoa(j));

		if (j == delim_len)
		{
			return i;
		}
	}
	return -1;
}

inline int lizzz_functions::explode_twice(std::vector< std::string > &output, std::string str, std::string delim)
{
	output.clear();
	int pos = str.find(delim);
	if (pos == std::string::npos)
	{
		output.push_back(str);
		return 1;
	}

	output.push_back(str.substr(0, pos));
	output.push_back(str.substr(pos + delim.length(), str.length() - pos));

	return 1;
}


static int find_pos2(std::string buff, std::string delim, int offset = 0)
{
	int buff_len = buff.length();
	int delim_len = delim.length();
	
	//printf("offset %d\r\n", offset);

	for (int i = offset; i < buff_len; i++)
	{
		int j;
		for (j = 0; j < delim_len; j++)
		{
			if (buff[i + j] != delim[j])
				break;
		}

		if (j >= delim_len)
		{
			return i;
		}
	}
	return -1;
}

static int copyBuff(std::string &out, std::string source, int offset, int len)
{
	int newSize = len - offset;
	out.resize(newSize);
	
	int n = 0;
	for(int l = offset; l < len; l++)
	{
		out[n++] = source[l];
	}
	

	//printf("offset %d pos %d %s\r\n", offset, len, out.c_str());
	
	return newSize;
}

inline int lizzz_functions::lizzz_explode(std::vector< std::string > &output, std::string str, std::string delim)
{
	output.clear();

	int i = 0;
	
	std::string part;
		
	int offset = 0;
	while (1)
	{
		int pos = find_pos2(str, delim, offset);
		if(pos == -1) break;
		
		copyBuff(part, str, offset, pos);
		

		//printf("offset %d pos %d %s\r\n", offset, pos, part.c_str());

		output.push_back(part);
		
		offset = pos + delim.size();
	}
	
	copyBuff(part, str, offset, str.size());
	output.push_back(part);


	return 1;
}

/*
inline int lizzz_functions::lizzz_explode(std::vector< std::string > &output, std::string str, const char *delim)
{
	output.clear();

	//const char *buf = str.c_str();
	char* buf = (char*)malloc(str.length() + 1);
	char* pointer_to_delete = buf;
	memcpy(buf, str.data(), str.length());
	buf[str.length()] = 0;
	

	int buf_len = str.length();
	int delim_len = strlen(delim);
	//Log("log.txt", "len " + _itoa(buf_len));

	//const char *st = "privet kak dela?";

	//const char* pos2 = strstr(st, "kak");
	//Log("log.txt", std::string("len ") + pos2);
	
	int countItem = 0;

	int pos = 0;
	while ((pos = find_pos(buf, delim)) != -1)
	{
		
		char* tmp = (char*)malloc(pos + 1);


		memcpy(tmp, buf, pos);
		tmp[pos] = 0;
		
		std::string item(tmp);
		//Log("log.txt", std::string("pos ") + tmp);
		
		delete tmp;
		output.push_back(item);
		
		buf = buf + pos + delim_len; //lizzz_Log::Instance()->addLog("log.txt", buf);
		
		countItem++;
	}

	int len_end = strlen(buf);
	char* end = (char*)malloc(len_end + 1);
	memcpy(end, buf, len_end);
	end[len_end] = 0;
	
	countItem++;
	
	std::string item(end);

	output.push_back(item);

	delete pointer_to_delete;


	return countItem;
}
*/

inline std::vector< char* > lizzz_functions::explode_fast(std::string str, const char *delim)
{
	//str = "privet\nkak\ndela?";
	std::vector< char* > result;

	//const char *buf = str.c_str();
	char* buf = (char*)malloc(str.length() + 1);
	char* pointer_to_delete = buf;
	memcpy(buf, str.data(), str.length());
	buf[str.length()] = 0;
	

	int buf_len = str.length();
	int delim_len = strlen(delim);
	//Log("log.txt", "len " + _itoa(buf_len));

	//const char *st = "privet kak dela?";

	//const char* pos2 = strstr(st, "kak");
	//Log("log.txt", std::string("len ") + pos2);

	int pos = 0;
	while ((pos = find_pos(buf, delim)) != -1)
	{
		char* tmp = (char*)malloc(pos + 1);


		memcpy(tmp, buf, pos);
		tmp[pos] = 0;
		//Log("log.txt", std::string("pos ") + tmp);

		result.push_back(tmp);
		
		buf = buf + pos + delim_len; //lizzz_Log::Instance()->addLog("log.txt", buf);
	}

	int len_end = strlen(buf);
	char* end = (char*)malloc(len_end + 1);
	memcpy(end, buf, len_end);
	end[len_end] = 0;

	result.push_back(end);

	delete pointer_to_delete;


	return result;
}

inline std::vector<std::string> lizzz_functions::explode(std::string str, const char *delim)
{
	std::vector<std::string> result;

	str += delim;



	while (true)
	{

		int pos = str.find(delim);
		if (pos == std::string::npos)
		{
			break;
		}

		if (str.length() == 0)
		{
			break;
		}

		//myprintf("pos %d\r\n", pos);

		std::string part = str.substr(0, pos);

		//myprintf("part %s\r\n", part.c_str());
		result.push_back(part);

		pos = pos + strlen(delim);
		str = str.substr(pos, str.length() - pos);
		//myprintf("res %s\r\n", str.c_str());

		//break;
	}


	return result;
}