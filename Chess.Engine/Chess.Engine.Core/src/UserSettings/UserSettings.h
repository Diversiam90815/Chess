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


class UserSettings
{
public:
	UserSettings()	= default;
	~UserSettings() = default;

	void init();

	template <typename T>
	void storeSetting(SettingsType setting, T value);

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

	void		   setMasterAudioVolume(const float volume);
	float		   getMasterVolume();

	void		   setAtmosScenario(const std::string scenario);
	std::string	   getAtmosScenario();

	void		   setDiscoveryPort(const int udpPort);
	int			   getDiscoveryPort();

	void		   setNetworkAdapter(const NetworkAdapter &adapter);
	NetworkAdapter getNetworkAdapter() const;

private:
	bool doesConfigFileExist();

	template <typename T>
	T				readOrDefault(const std::string &fileKey, const T &defaultValue, SettingsType setting, const std::string &logName);

	void			initializeConfigFile(DefaultSettings settings);

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
		fmg->writeSettingToFile<T>(BoardStyleSetting, value);
		break;
	}
	case SettingsType::ChessPieceStyle:
	{
		fmg->writeSettingToFile<T>(PieceStyleSetting, value);
		break;
	}
	case SettingsType::PlayerName:
	{
		fmg->writeSettingToFile<T>(PlayerNameSetting, value);
		break;
	}
	case SettingsType::AudioSFXEnabled:
	{
		fmg->writeSettingToFile<T>(AudioSFXEnabledSetting, value);
		break;
	}
	case SettingsType::AudioSFXVolume:
	{
		fmg->writeSettingToFile<T>(AudioSFXVolumeSetting, value);
		break;
	}
	case SettingsType::AudioAtmosEnabled:
	{
		fmg->writeSettingToFile<T>(AudioAtmosEnabledSetting, value);
		break;
	}
	case SettingsType::AudioAtmosVolume:
	{
		fmg->writeSettingToFile<T>(AudioAtmosVolumeSetting, value);
		break;
	}
	case SettingsType::AudioAtmosScenario:
	{
		fmg->writeSettingToFile<T>(AudioAtmosScenarioSetting, value);
		break;
	}
	case SettingsType::AudioMasterVolume:
	{
		fmg->writeSettingToFile<T>(AudioMasterVolumeSetting, value);
		break;
	}
	case SettingsType::DiscoveryUDPPort:
	{
		fmg->writeSettingToFile<T>(DiscoveryUDPPortSetting, value);
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
			value = readOrDefault<std::string>(BoardStyleSetting, mDefaultSettings.BoardStyle, setting, "Board Style");
		}
		break;
	}
	case (SettingsType::ChessPieceStyle):
	{
		if constexpr (std::is_same_v<T, std::string>)
		{
			value = readOrDefault<std::string>(PieceStyleSetting, mDefaultSettings.ChessPieceStyle, setting, "Piece Style");
		}
		break;
	}
	case (SettingsType::PlayerName):
	{
		if constexpr (std::is_same_v<T, std::string>)
		{
			value = readOrDefault<std::string>(PlayerNameSetting, {}, setting, "Local Player Name");
		}
		break;
	}
	case SettingsType::AudioSFXEnabled:
	{
		value = readOrDefault<bool>(AudioSFXEnabledSetting, mDefaultSettings.SFXEnabled, setting, "Audio SFX Enabled");
		break;
	}
	case SettingsType::AudioSFXVolume:
	{
		value = readOrDefault<float>(AudioSFXVolumeSetting, mDefaultSettings.SFXVolume, setting, "Audio SFX Volume");
		break;
	}
	case SettingsType::AudioAtmosEnabled:
	{
		value = readOrDefault<bool>(AudioAtmosEnabledSetting, mDefaultSettings.AtmosEnabled, setting, "Audio Atmos Enabled");
		break;
	}
	case SettingsType::AudioAtmosVolume:
	{
		value = readOrDefault<float>(AudioAtmosVolumeSetting, mDefaultSettings.AtmosVolume, setting, "Audio Atmos Volume");
		break;
	}
	case SettingsType::AudioAtmosScenario:
	{
		if constexpr (std::is_same_v<T, std::string>)
		{
			value = readOrDefault<std::string>(AudioAtmosScenarioSetting, mDefaultSettings.AtmosScenario, setting, "Audio Atmos Scencario");
		}
		break;
	}
	case SettingsType::AudioMasterVolume:
	{
		value = readOrDefault<float>(AudioMasterVolumeSetting, mDefaultSettings.MasterVolume, setting, "Audio Master Volume");
		break;
	}
	case SettingsType::DiscoveryUDPPort:
	{
		value = readOrDefault<int>(DiscoveryUDPPortSetting, mDefaultSettings.DiscoveryUDPPort, setting, "Discovery UDP Port");
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
