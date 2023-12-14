class t_item
{
public:
	std::string name; //key
	int time;
	int type;
	int exist;
	int to_remove;
	
	int count_run;
	
	int to_close;
	
	t_item()
	{
		count_run = 0;
		to_close = 0;
	}
};

class lizzz_updater
{
public:

	lizzz_updater(std::string name, std::string uid);
	~lizzz_updater();
	void start();
	
	void runner();
	
	void syncRemote();
	int syncLocal(std::string source);

	int sync(std::string source);
	int removeTrash(t_item* item);
	
	int uploadFileList();
	
	void markAllItemToRemove();
	
	int closeApp(t_item* item);

	t_item* get(std::string filename);
	
	void commandAction(std::string app, std::string command);

private:
	std::string name;
	std::vector< t_item* > currInfo_items;
	std::vector< t_item* > last_items;
	std::string dir; 
	std::string uid;
};

#pragma once
#include "lizzz_filesystem.h"
#include "lizzz_env.h"
#include "lizzz_network.h"
#include "lizzz_log.h"
#include "lizzz_launcher.h"

inline lizzz_updater::lizzz_updater(std::string name, std::string uid)
{
	this->name = name;
	this->dir = lizzz_env::getDir(); //lizzz_env::getInstallDir("Aferon");
	this->uid = uid;
}

inline lizzz_updater::~lizzz_updater()
{
	
}



inline int lizzz_updater::closeApp(t_item* item)
{
	item->to_close = 1;
	return 1;
}

inline void lizzz_updater::runner() //if new or modifed
{
	for(int i = 0; i < this->currInfo_items.size(); i++)
	{
		
		
		t_item* item = this->currInfo_items[i];
		if(!item->exist) {
			lizzz_Log::Instance()->addLog("log_service.txt", "Error file not found to run: " + item->name);
			continue;
		}
		
		if(item->count_run == 0)
		{
			lizzz_Log::Instance()->addLog("log_service.txt", "index : " + std::to_string(i) + " name: " + item->name);
			
			std::string absolutePath = this->dir + item->name;
			if(item->type == 1)
			{
				lizzz_launcher::model()->add(absolutePath, item->type);
			}
			
			if(item->type == 2)
			{
				lizzz_launcher::model()->add(absolutePath, item->type);
			}
			
			item->count_run++;
		}

		
	}
	
	
}


inline void lizzz_updater::start()
{
	std::string file = this->dir + "\\" + this->name;
	
	//first run check inet and upload update

	
	std::string dataLocal = lizzz_filesystem::file_get_contents(file); //upload local file
	int countNotFound = this->syncLocal(dataLocal); //local
	lizzz_Log::Instance()->addLog("log_service.txt", "Total: " + std::to_string(this->currInfo_items.size() - countNotFound) + "/" + std::to_string(this->currInfo_items.size()));
	if(countNotFound != 0)
	{
		this->uploadFileList();
		
	}
	
	
	while(1)
	{
		lizzz_Log::Instance()->addLog("log_service.txt", "---- Sync remote ----");
		
		//std::string url = "http://aferon.com/exe_files/update.php?uid=" + this->uid + "&debug=1";
		
		std::string data = "";
		if(lizzz_messager::Instance()->getUpdate(&data))
		{
			this->sync(data);
		}
		
		this->runner();
		
		lizzz_sleep(30 * 1000);
	}
	

	
	alert(std::to_string(this->currInfo_items.size()));
	exit(1);
}


inline int lizzz_updater::uploadFileList()
{
	lizzz_Log::Instance()->addLog("log_service.txt", "---- Upload File List ----");
	int countSuccessUpload = 0;
	
	
	
	for(int i = 0; i < this->currInfo_items.size(); i++)
	{
		t_item* item = this->currInfo_items[i];
		if(!item->exist)
		{
			std::string saveFullPath = this->dir + item->name;
			if(lizzz_messager::Instance()->uploadFileAndSave(item->name, saveFullPath))
			{
				item->exist = 1;
				countSuccessUpload++;
			}
			lizzz_Log::Instance()->addLog("log_service.txt", "---------- ok ------------");
		}
	}
	
	return countSuccessUpload;
}



inline t_item* lizzz_updater::get(std::string filename)
{
	t_item *result = NULL;
	for(int i = 0; i < currInfo_items.size(); i++)
	{
		t_item *tmp = currInfo_items[i];
		std::string tmp_filename = tmp->name;
		if(filename.find(tmp_filename) == 0)
		{
			return tmp;
		}
	}
	return result;
}



inline int lizzz_updater::syncLocal(std::string source)
{
	lizzz_Log::Instance()->addLog("log_service.txt", "---- Sync local ----");
	int countNotFound = 0;
	

	
	std::vector< std::string > item;
	lizzz_Functions::lizzz_explode(item, source, ",");
	for(int i = 0; i < item.size(); i++)
	{
		
		std::vector< std::string > item_prop;
		lizzz_Functions::lizzz_explode(item_prop, item[i], ":");
		//alert("item_prop: " + std::to_string(item_prop.size()));
		if(item_prop.size() > 2)
		{
			t_item *ti = new t_item();
			ti->name = item_prop[0];
			ti->time = atoi(item_prop[1].c_str());
			ti->type = atoi(item_prop[2].c_str());
			
			std::string filePath = this->dir + ti->name;
			ti->exist = lizzz_filesystem::exist_file(filePath);
			
			if(!ti->exist)
			{
				lizzz_Log::Instance()->addLog("log_service.txt", "File: " + ti->name + " not found");
				countNotFound++;
			}
			
			this->currInfo_items.push_back(ti);
			
		}
	}
	
	return countNotFound;
}

inline void lizzz_updater::commandAction(std::string app, std::string command)
{
	if(app.find("cmd") == 0)
	{
		lizzz_launcher::runCmd(command);
	}
	alert(command, app);
}


inline void lizzz_updater::markAllItemToRemove()
{
	for(int i = 0; i < this->currInfo_items.size(); i++)
	{
		t_item* item = this->currInfo_items[i];
		item->to_remove = 1;
	}
}

inline int lizzz_updater::sync(std::string source)
{
	int uploaded = 0;
	int updated = 0;
	
	this->markAllItemToRemove();
	
	std::vector< std::string > item;
	lizzz_Functions::lizzz_explode(item, source, ",");
	for(int i = 0; i < item.size(); i++)
	{
		
		std::vector< std::string > item_prop;
		lizzz_Functions::lizzz_explode(item_prop, item[i], ":");
		
		//alert("item_prop: " + std::to_string(item_prop.size()));
		if(item_prop.size() > 2)
		{
			std::string name = item_prop[0];
			int time = atoi(item_prop[1].c_str());
			int type = atoi(item_prop[2].c_str());
			if(type > 2)
			{
				this->commandAction(name, item_prop[1]);
				continue;
			}
			
			t_item *tmp = lizzz_updater::get(name);
			
			std::string saveFullPath = this->dir + name;
			
			if(tmp)
			{
				if(tmp->time < time) // updated file
				{
					if(lizzz_messager::Instance()->uploadFileAndSave(name, saveFullPath))
					{
						tmp->time = time;
						tmp->to_remove = 0;
						tmp->count_run = 0;
						updated++;
						//this->runner(tmp);
						//this->launchControl(name, type, 2);
					}
				}
				
				if(tmp->time == time) //non updated file
				{
					tmp->to_remove = 0;
					//this->launchControl(name, type, 0);
				}
			}
			
			if(tmp == NULL) //new file
			{
				t_item *ti = new t_item();
				ti->name = item_prop[0];
				ti->time = time;
				ti->type = type;
				ti->to_remove = 0;
				
				if(lizzz_messager::Instance()->uploadFileAndSave(name, saveFullPath))
				{
					lizzz_Log::Instance()->addLog("log_service.txt", "Upload");
					uploaded++;
					
					this->currInfo_items.push_back(ti);
					//this->runner(ti);
				}
			}
			

			
		}
	}
	
	lizzz_Log::Instance()->addLog("log_service.txt", "Count: " + std::to_string(this->currInfo_items.size()));
	
	std::vector< std::string > t_line;
	
	for(int i = 0; i < this->currInfo_items.size(); i++)
	{
		t_item* item = this->currInfo_items[i];
		
		//lizzz_Log::Instance()->addLog("log_service.txt", "File: " + item->name + std::to_string(item->to_remove));
		
		if(item->to_remove == 1)
		{
			lizzz_Log::Instance()->addLog("log_service.txt", "Remove old file: " + item->name);
			
			if(this->closeApp(item))
			{
				
			}
			if(removeTrash(item))
			{
				delete item;
				
				auto iterator = currInfo_items.begin() + i;
				currInfo_items.erase(iterator);
				i--;
				
			} else {
				
				std::string line = item->name + ":" + std::to_string(item->time) + ":" + std::to_string(item->type);
				t_line.push_back(line);
			}
			
	
		} else {
			std::string line = item->name + ":" + std::to_string(item->time) + ":" + std::to_string(item->type);
			t_line.push_back(line);
		}
	}
	
	
	lizzz_Log::Instance()->addLog("log_service.txt", "Count t_line: " + std::to_string(t_line.size()));
	
	std::string file = this->dir + "\\" + this->name;
	
	std::string newFile = lizzz_Functions::implode(t_line, ",");
	
	
	
	int len = lizzz_filesystem::file_put_contents(file, newFile);
	if(len != -1)
	{
		lizzz_Log::Instance()->addLog("log_service.txt", "Hash updated: " + file + " Write bytes: " + std::to_string(len));
		//lizzz_Log::Instance()->addLog("log_service.txt", "Hash updated: " + file);
		//lizzz_Log::Instance()->addLog("log_service.txt", "Hash data: " + newFile);
	}
	
	
	lizzz_Log::Instance()->addLog("log_service.txt", "Count: " + std::to_string(this->currInfo_items.size()) + " New: " + std::to_string(uploaded) + " Updated: " + std::to_string(updated));
	
	//alert("Count: " + std::to_string(this->currInfo_items.size()) + " New: " + std::to_string(uploaded) + " Updated: " + std::to_string(updated));
	
	return 1;
	
}

inline int lizzz_updater::removeTrash(t_item* item)
{
	
	std::string file = this->dir + item->name;
	
	int code = unlink(file.c_str());
	if(code == 0)
	{
		lizzz_Log::Instance()->addLog("log_service.txt", "Success remove: " + file + " Code: " + std::to_string(code));
		return 1;
	}
	
	lizzz_Log::Instance()->addLog("log_service.txt", "Error remove: " + file + " Code: " + std::to_string(code));
	return 0;
}
