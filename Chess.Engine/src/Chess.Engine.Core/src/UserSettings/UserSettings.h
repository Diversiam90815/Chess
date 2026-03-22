/*
  ==============================================================================
	Module:         UserSetting
	Description:    Storing and managing the settings set by the user
  ==============================================================================
*/

#pragma once

#include <optional>

#include "FileManager.h"
#include "Logging.h"
#include "Parameters.h"

/// <summary>
/// Represents the default configuration settings for a chess application.
/// If no config was found, these settings will take effect.
/// </summary>
struct DefaultSettings
{
	const std::string BoardStyle	   = "Wood";
	const std::string ChessPieceStyle  = "Basic";

	const std::string AtmosScenario	   = "Forest";
	const float		  AtmosVolume	   = 1.0f;
	const float		  SFXVolume		   = 1.0f;
	const float		  MasterVolume	   = 1.0f;
	const bool		  SFXEnabled	   = true;
	const bool		  AtmosEnabled	   = true;

	const int		  DiscoveryUDPPort = 5555;
};


/**
 * @brief Manages user-specific application settings (themes, player name, audio,
 *        networking). Provides typed store / read helpers that fallback to defaults.
 *
 * Persistence strategy:
 *  - On first run (no config file) a default config is created.
 *  - Individual setters update in-memory values and persist immediately.
 *
 * Thread-safety: Not inherently thread-safe; external synchronization required
 * if accessed from multiple threads concurrently.
 */
class UserSettings
{
public:
	UserSettings()	= default;
	~UserSettings() = default;

	/**
	 * @brief	Initialize settings storage (creates config file if missing, loads values).
	 *			Safe to call multiple times; subsequent calls are no-ops.
	 */
	void init();

	/**
	 * @brief	Store a typed setting value.
	 * @tparam	T		->	Serializable value type.
	 * @param	setting	->	Enum key.
	 * @param	value	->	Value to persist.
	 */
	template <typename T>
	void storeSetting(SettingsType setting, T value);

	/**
	 * @brief	Read a typed setting value.
	 * @tparam	T		->	Target type (must match stored type).
	 * @param	setting	->	Enum key.
	 * @return	Stored value or default if missing / type mismatch.
	 */
	template <typename T>
	T			   readSetting(SettingsType setting);

	void		   setCurrentBoardTheme(std::string theme);
	std::string	   getCurrentBoardTheme();

	void		   setCurrentPieceTheme(std::string theme);
	std::string	   getCurrentPieceTheme();

	void		   setLocalPlayerName(const std::string name);
	std::string	   getLocalPlayerName();

	void		   setSFXEnabled(const bool enabled);
	bool		   getSFXEnabled();

	void		   setAtmosEnabled(const bool enabled);
	bool		   getAtmosEnabled();

	void		   setSFXVolume(const float volume);
	float		   getSFXVolume();

	void		   setAtmosVolume(const float volume);
	float		   getAtmosVolume();

	void		   setMasterVolume(const float volume);
	float		   getMasterVolume();

	void		   setAtmosScenario(const std::string scenario);
	std::string	   getAtmosScenario();

	void		   setDiscoveryPort(const int udpPort);
	int			   getDiscoveryPort();

	void		   setNetworkAdapter(const NetworkAdapter &adapter);
	NetworkAdapter getNetworkAdapter() const;

private:
	/**
	 * @brief	Check for existing configuration file on disk.
	 */
	bool doesConfigFileExist();

	/**
	 * @brief	Read typed value from config file or return provided default.
	 * @tparam	T	->	Desired type.
	 * @param	fileKey Key	-> string in storage.
	 * @param	defaultValue -> Fallback value.
	 * @param	setting ->	Enum (for logging / mapping).
	 * @param	logName	->	Friendly name for diagnostics.
	 */
	template <typename T>
	T				readOrDefault(const std::string &fileKey, const T &defaultValue, SettingsType setting, const std::string &logName);

	/**
	 * @brief	Create initial configuration file using provided defaults.
	 */
	void			initializeConfigFile(DefaultSettings settings);

	/**
	 * @brief	Emit current effective settings to log (debug / support aid).
	 */
	void			logUserSettings();


	DefaultSettings mDefaultSettings;
};


template <typename T>
inline void UserSettings::storeSetting(SettingsType setting, T value)
{
	FileManager *fmg = FileManager::GetInstance();

	switch (setting)
	{
	case SettingsType::BoardStyle:
	{
		fmg->writeSettingToFile<T>(SettingName::BoardStyleSetting, value);
		break;
	}
	case SettingsType::ChessPieceStyle:
	{
		fmg->writeSettingToFile<T>(SettingName::PieceStyleSetting, value);
		break;
	}
	case SettingsType::PlayerName:
	{
		fmg->writeSettingToFile<T>(SettingName::PlayerNameSetting, value);
		break;
	}
	case SettingsType::AudioSFXEnabled:
	{
		fmg->writeSettingToFile<T>(SettingName::AudioSFXEnabledSetting, value);
		break;
	}
	case SettingsType::AudioSFXVolume:
	{
		fmg->writeSettingToFile<T>(SettingName::AudioSFXVolumeSetting, value);
		break;
	}
	case SettingsType::AudioAtmosEnabled:
	{
		fmg->writeSettingToFile<T>(SettingName::AudioAtmosEnabledSetting, value);
		break;
	}
	case SettingsType::AudioAtmosVolume:
	{
		fmg->writeSettingToFile<T>(SettingName::AudioAtmosVolumeSetting, value);
		break;
	}
	case SettingsType::AudioAtmosScenario:
	{
		fmg->writeSettingToFile<T>(SettingName::AudioAtmosScenarioSetting, value);
		break;
	}
	case SettingsType::AudioMasterVolume:
	{
		fmg->writeSettingToFile<T>(SettingName::AudioMasterVolumeSetting, value);
		break;
	}
	case SettingsType::DiscoveryUDPPort:
	{
		fmg->writeSettingToFile<T>(SettingName::DiscoveryUDPPortSetting, value);
		break;
	}

	default: break;
	}
}


template <typename T>
inline T UserSettings::readSetting(SettingsType setting)
{
	T value = {};

	switch (setting)
	{
	case (SettingsType::BoardStyle):
	{
		if constexpr (std::is_same_v<T, std::string>)
		{
			value = readOrDefault<std::string>(SettingName::BoardStyleSetting, mDefaultSettings.BoardStyle, setting, "Board Style");
		}
		break;
	}
	case (SettingsType::ChessPieceStyle):
	{
		if constexpr (std::is_same_v<T, std::string>)
		{
			value = readOrDefault<std::string>(SettingName::PieceStyleSetting, mDefaultSettings.ChessPieceStyle, setting, "Piece Style");
		}
		break;
	}
	case (SettingsType::PlayerName):
	{
		if constexpr (std::is_same_v<T, std::string>)
		{
			value = readOrDefault<std::string>(SettingName::PlayerNameSetting, {}, setting, "Local Player Name");
		}
		break;
	}
	case SettingsType::AudioSFXEnabled:
	{
		value = readOrDefault<bool>(SettingName::AudioSFXEnabledSetting, mDefaultSettings.SFXEnabled, setting, "Audio SFX Enabled");
		break;
	}
	case SettingsType::AudioSFXVolume:
	{
		value = readOrDefault<float>(SettingName::AudioSFXVolumeSetting, mDefaultSettings.SFXVolume, setting, "Audio SFX Volume");
		break;
	}
	case SettingsType::AudioAtmosEnabled:
	{
		value = readOrDefault<bool>(SettingName::AudioAtmosEnabledSetting, mDefaultSettings.AtmosEnabled, setting, "Audio Atmos Enabled");
		break;
	}
	case SettingsType::AudioAtmosVolume:
	{
		value = readOrDefault<float>(SettingName::AudioAtmosVolumeSetting, mDefaultSettings.AtmosVolume, setting, "Audio Atmos Volume");
		break;
	}
	case SettingsType::AudioAtmosScenario:
	{
		if constexpr (std::is_same_v<T, std::string>)
		{
			value = readOrDefault<std::string>(SettingName::AudioAtmosScenarioSetting, mDefaultSettings.AtmosScenario, setting, "Audio Atmos Scencario");
		}
		break;
	}
	case SettingsType::AudioMasterVolume:
	{
		value = readOrDefault<float>(SettingName::AudioMasterVolumeSetting, mDefaultSettings.MasterVolume, setting, "Audio Master Volume");
		break;
	}
	case SettingsType::DiscoveryUDPPort:
	{
		value = readOrDefault<int>(SettingName::DiscoveryUDPPortSetting, mDefaultSettings.DiscoveryUDPPort, setting, "Discovery UDP Port");
		break;
	}
	default: return value;
	}

	return value;
}


template <typename T>
inline T UserSettings::readOrDefault(const std::string &fileKey, const T &defaultValue, SettingsType setting, const std::string &logName)
{
	FileManager *fileManager = FileManager::GetInstance();
	auto		 optValue	 = fileManager->readSettingFromFile<T>(fileKey);

	if (!optValue.has_value())
	{
		// If empty, use current member as default and store it
		storeSetting<T>(setting, defaultValue);
		LOG_WARNING("Since setting {} failed to read from config file, we set the default and store it!", logName);
		return defaultValue;
	}

	return optValue.value();
}
