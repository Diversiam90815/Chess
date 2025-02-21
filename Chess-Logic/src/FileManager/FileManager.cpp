/*
  ==============================================================================
	Module:         FileManager
	Description:    Singleton of the file system manager (AppData path's need to be set from UI project due to WinUi's virtualization.
  ==============================================================================
*/

#include "FileManager.h"
#include <iostream>

void to_json(json &j, const NetworkAdapter &adapter)
{
	j = json{{"ID", adapter.ID},		 {"Description", adapter.description}, {"IPv4", adapter.IPv4},
			 {"Subnet", adapter.subnet}, {"Eligible", adapter.eligible},	   {"Selected", adapter.selected}};
}

void from_json(const json &j, NetworkAdapter &adapter)
{
	j.at("ID").get_to(adapter.ID);
	j.at("Description").get_to(adapter.description);
	j.at("IPv4").get_to(adapter.IPv4);
	j.at("Subnet").get_to(adapter.subnet);
	j.at("Eligible").get_to(adapter.eligible);
	j.at("Selected").get_to(adapter.selected);
}


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
