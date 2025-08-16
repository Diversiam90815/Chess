/*
  ==============================================================================
	Module:         FileManager
	Description:    Singleton of the file system manager (AppData path's need to be set from UI project due to WinUi's virtualization.
  ==============================================================================
*/

#pragma once

#include <filesystem>
#include <json.hpp>
#include <fstream>
#include <optional>

#include "Parameters.h"
#include "Logging.h"
#include "NetworkAdapter.h"
#include "JsonConversion.h"


namespace fs = std::filesystem;


class FileManager
{
public:
	~FileManager() = default;

	// Singleton instance
	static FileManager *GetInstance();
	static void			ReleaseInstance();

	// Getter - Setter
	fs::path			getAppDataPath();
	void				setAppDataPath(std::string path);

	// Path retrieval functions
	fs::path			getLoggingPath();
	fs::path			getSettingsPath();
	fs::path			getUserSettingsPath();

	template <typename T>
	std::optional<T> readSettingFromFile(const std::string &setting);

	template <typename T>
	bool						  writeSettingToFile(const std::string &setting, const T &value);


	std::optional<NetworkAdapter> readSelectedNetworkAdapter();
	bool						  setSelectedNetworkAdapter(const NetworkAdapter &adpater);

private:
	FileManager() = default;

	bool		createDirectoryIfNeeded(const fs::path &directory);

	std::string mAppDataPath = "";
};


template <typename T>
inline std::optional<T> FileManager::readSettingFromFile(const std::string &setting)
{
	try
	{
		fs::path	  configPath = getUserSettingsPath();

		std::ifstream jsonIN(configPath);

		if (!jsonIN)
		{
			LOG_WARNING("Failed to open config file {}", configPath.string().c_str());
			return std::nullopt;
		}

		json config;
		jsonIN >> config;

		if (config.contains(setting))
		{
			return config[setting].get<T>();
		}
		else
		{
			LOG_WARNING("Setting {} not found in config file.", setting.c_str());
			return std::nullopt;
		}
	}
	catch (const std::exception &e)
	{
		LOG_WARNING("Exception occured during reading from JSON: {}", e.what());
		return std::nullopt;
	}
}


template <typename T>
inline bool FileManager::writeSettingToFile(const std::string &setting, const T &value)
{
	try
	{
		fs::path configPath = getUserSettingsPath();
		json	 userConfig;

		if (fs::exists(configPath))
		{
			std::ifstream jsonIn(configPath);
			if (jsonIn)
			{
				jsonIn >> userConfig;
			}
			else
			{
				LOG_WARNING("Failed to open existing config file {}", configPath.string().c_str());
			}
		}

		userConfig[setting] = value;

		std::ofstream jsonOut(configPath);
		if (!jsonOut)
		{
			LOG_ERROR("Failed to open file {} for writing", configPath.string().c_str());
			return false;
		}

		jsonOut << std::setw(4) << userConfig;
		return true;
	}
	catch (const std::exception &e)
	{
		LOG_WARNING("Exception occurred during writing to JSON: {}", e.what());
		return false;
	}
}
