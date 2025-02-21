/*
  ==============================================================================
	Module:         FileManager
	Description:    Singleton of the file system manager (AppData path's need to be set from UI project due to WinUi's virtualization.
  ==============================================================================
*/

#include "FileManager.h"
#include <iostream>


FileManager *FileManager::GetInstance()
{
	static FileManager *sInstance = nullptr;
	if (nullptr == sInstance)
	{
		sInstance = new FileManager();
	}
	return sInstance;
}


void FileManager::ReleaseInstance()
{
	FileManager *sInstance = GetInstance();
	if (sInstance)
	{
		delete sInstance;
	}
}


fs::path FileManager::getAppDataPath()
{
	return fs::path(mAppDataPath);
}


void FileManager::setAppDataPath(std::string path)
{
	mAppDataPath = path;
}


fs::path FileManager::getLoggingPath()
{
	fs::path path = getAppDataPath() / LoggingFolder;
	createDirectoryIfNeeded(path);
	return path;
}


fs::path FileManager::getSettingsPath()
{
	fs::path path = getAppDataPath() / SettingsFolder;
	createDirectoryIfNeeded(path);
	return path;
}


fs::path FileManager::getUserSettingsPath()
{
	fs::path path = getSettingsPath() / UserSettingsFile;
	return path;
}


std::string FileManager::readSettingFromFile(const std::string &setting)
{
	try
	{
		fs::path	  configPath = getUserSettingsPath();

		std::ifstream jsonIN(configPath);

		if (!jsonIN)
		{
			LOG_WARNING("Failed to open config file {}", configPath.string().c_str());
			return "";
		}

		json config;
		jsonIN >> config;

		if (config.contains(setting))
		{
			return config[setting].get<std::string>();
		}
		else
		{
			LOG_WARNING("Setting {} not found in config file.", setting.c_str());
			return "";
		}
	}
	catch (const std::exception &e)
	{
		LOG_WARNING("Exception occured during reading from JSON: {}", e.what());
	}
}


bool FileManager::writeSettingToFile(const std::string &setting, const std::string &value)
{
	try
	{
		fs::path configPath = getUserSettingsPath();
		json	 userConfig;

		if (fs::exists(configPath))
		{
			std::ifstream jsonIN(configPath);
			if (jsonIN)
			{
				jsonIN >> userConfig;
			}
			else
			{
				LOG_WARNING("Failed to open existing config file {}", configPath.string().c_str());
				return false;
			}
		}

		userConfig[setting] = value;

		std::ofstream jsonOUT(configPath);
		if (!jsonOUT)
		{
			LOG_WARNING("Failed to open file {} for writing", configPath.string().c_str());
			return false;
		}

		jsonOUT << std::setw(4) << userConfig;
		return true;
	}
	catch (const std::exception &e)
	{
		LOG_WARNING("Exception occured during writing to JSON: {}", e.what());
		return false;
	}
	return false;
}


bool FileManager::createDirectoryIfNeeded(const fs::path &directory)
{
	std::error_code ec;
	if (!fs::exists(directory) && !fs::create_directories(directory, ec))
	{
		std::cerr << "Failed to create directory!" << std::endl;
		return false;
	}
	return true;
}
