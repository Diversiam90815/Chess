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
	const std::string BoardStyle = "Wood";
	const std::string ChessPieceStyle = "Basic";
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
	T			readSetting(SettingsType setting);


	void		setCurrentBoardTheme(std::string theme);
	std::string getCurrentBoardTheme();

	void		setCurrentPieceTheme(std::string theme);
	std::string getCurrentPieceTheme();

	void		setLocalPlayerName(const std::string name);
	std::string getLocalPlayerName();


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
		fmg->writeSettingToFile<std::string>(BoardStyleSetting, value);
		break;
	}
	case SettingsType::ChessPieceStyle:
	{
		fmg->writeSettingToFile<std::string>(PieceStyleSetting, value);
		break;
	}
	case SettingsType::PlayerName:
	{
		fmg->writeSettingToFile<std::string>(PlayerNameSetting, value);
		break;
	}
	default: break;
	}
}


template <typename T>
inline T UserSettings::readSetting(SettingsType setting)
{
	T value = "";

	switch (setting)
	{
	case (SettingsType::BoardStyle):
	{
		value = readOrDefault<std::string>(BoardStyleSetting, mDefaultSettings.BoardStyle, setting, "Board Style");
		return value;
	}
	case (SettingsType::ChessPieceStyle):
	{
		value = readOrDefault<std::string>(PieceStyleSetting, mDefaultSettings.ChessPieceStyle, setting, "Piece Style");
		return value;
	}
	case (SettingsType::PlayerName):
	{
		value = readOrDefault<std::string>(PlayerNameSetting, "", setting, "Local Player Name");
		return value;
	}
	default: return value;
	}
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
