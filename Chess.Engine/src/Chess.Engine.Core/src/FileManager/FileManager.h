/*
  ==============================================================================
	Module:         FileManager
	Description:    Singleton of the file system manager (AppData path's need to be set from UI project due to WinUi's virtualization.
  ==============================================================================
*/

#pragma once

#include <filesystem>
#include <nlohmann/json.hpp>
#include <fstream>
#include <optional>

#include "Parameters.h"
#include "Logging.h"
#include "NetworkAdapter.h"
#include "JsonConversion.h"


namespace fs = std::filesystem;

/**
 * @brief	Manages application file paths and settings. Singleton accessor pattern.
			AppData path needs to be set once via the Engine.API due to the virtualization inside WinUi apps.
 *
 * Responsibilities:
 *  - Resolve and persist application data root path.
 *  - Provide well-known sub paths (logging, settings, user settings).
 *  - Read / write strongly typed settings stored in a settings file.
 *  - Persist selected network adapter information.
 */
class FileManager
{
public:
	~FileManager() = default;

	// Singleton instance
	static FileManager *GetInstance();
	static void			ReleaseInstance();

	/**
	 * @brief	Get root path used for application data (log/config/etc.).
	 * @return	Absolute path. May be created if not existing yet.
	 */
	fs::path			getAppDataPath();

	/**
	 * @brief	Override the application data root path.
	 * @param	path New path (created if missing).
	 */
	void				setAppDataPath(std::string path);

	/**
	 * @brief	Path where log files are stored.
	 */
	fs::path			getLoggingPath();

	/**
	 * @brief	Path of the main settings storage (e.g. JSON / config file).
	 */
	fs::path			getSettingsPath();

	/**
	 * @brief	Path of user specific (mutable) settings file.
	 */
	fs::path			getUserSettingsPath();

	/**
	 * @brief	Read a typed setting value from the user settings file.
	 * @tparam	T		->	Expected value type (must be deserializable).
	 * @param	setting	->	Key / field name.
	 * @return	std::nullopt if file or key missing or conversion failed.
	 */
	template <typename T>
	std::optional<T> readSettingFromFile(const std::string &setting);

	/**
	 * @brief	Write / update a typed setting value in the user settings file.
	 * @tparam	T Value type (must be serializable).
	 * @param	setting	->	Key / field name.
	 * @param	value	->	Value to persist.
	 * @return	true on success; false on I/O or serialization failure.
	 */
	template <typename T>
	bool						  writeSettingToFile(const std::string &setting, const T &value);

	/**
	 * @brief	Retrieve the previously persisted network adapter selection.
	 * @return	Adapter or std::nullopt if not stored.
	 */
	std::optional<NetworkAdapter> readSelectedNetworkAdapter();

	/**
	 * @brief	Persist the selected network adapter for later retrieval.
	 * @param	adapter -> Adapter descriptor.
	 * @return	true on success.
	 */
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
