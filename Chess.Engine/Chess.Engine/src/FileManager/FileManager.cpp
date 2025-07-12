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


std::optional<NetworkAdapter> FileManager::readSelectedNetworkAdapter()
{
	NetworkAdapter adapter{};
	fs::path	   configPath = getSettingsPath() / UserSettingsFile;

	if (fs::exists(configPath))
	{
		// Read the file in
		std::ifstream jsonIN(configPath);
		if (jsonIN)
		{
			nlohmann::json userConfig;
			jsonIN >> userConfig;

			// Set the adapter if available
			if (userConfig.contains(SelectedAdapter))
			{
				adapter = userConfig[SelectedAdapter].get<NetworkAdapter>();
				return adapter;
			}
		}
		else
		{
			LOG_WARNING("Failed to open the config file!");
		}
	}
	else
	{
		LOG_WARNING("Config file does not exist!");
	}
	return std::nullopt;
}


bool FileManager::setSelectedNetworkAdapter(const NetworkAdapter &adapter)
{
	try
	{
		fs::path configPath = getSettingsPath() / UserSettingsFile;
		json	 config;

		// Read the file in
		if (fs::exists(configPath))
		{
			std::ifstream jsonIN(configPath);
			if (jsonIN)
			{
				jsonIN >> config;
			}
			else
			{
				LOG_WARNING("Failed to open existing config file {}", configPath.string().c_str());
				return false;
			}
		}

		// Set the adapter (or add it)
		config[SelectedAdapter] = adapter;

		// Write the file
		std::ofstream jsonOUT(configPath);
		if (!jsonOUT)
		{
			LOG_WARNING("Failed to open file {} for writing", configPath.string().c_str());
			return false;
		}
		jsonOUT << std::setw(4) << config;
		return true;
	}
	catch (std::exception e)
	{
		LOG_WARNING("Exception occured during writing network adapter: {}", e.what());
		return false;
	}
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
