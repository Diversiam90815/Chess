/*
  ==============================================================================
	Module:         UserSetting
	Description:    Storing and managing the settings set by the user
  ==============================================================================
*/

#include "UserSettings.h"


void UserSettings::init()
{
	bool configFileExists = doesConfigFileExist();

	if (!configFileExists)
	{
		LOG_INFO("User Config not found, so we set up one!");
		initializeConfigFile(mDefaultSettings);
		return;
	}

	LOG_INFO("User Config found");
	logUserSettings();
}


void UserSettings::storeSetting(SettingsType setting, std::string value)
{
	switch (setting)
	{
	case (SettingsType::boardTheme):
	{
		FileManager *fmg = FileManager::GetInstance();
		fmg->writeSettingToFile(BoardTheme, value);
		break;
	}
	case (SettingsType::piecesTheme):
	{
		FileManager *fmg = FileManager::GetInstance();
		fmg->writeSettingToFile(PieceTheme, value);
		break;
	}
	case (SettingsType::playerName):
	{
		FileManager *fmg = FileManager::GetInstance();
		fmg->writeSettingToFile(PlayerNameSetting, value);
		break;
	}
	default: break;
	}
}


std::string UserSettings::readSetting(SettingsType setting)
{
	std::string value = "";

	switch (setting)
	{
	case (SettingsType::boardTheme):
	{
		FileManager *fmg = FileManager::GetInstance();
		value			 = fmg->readSettingFromFile(BoardTheme);
		LOG_INFO("Board theme read from file {}", value.c_str());
		return value;
	}
	case (SettingsType::piecesTheme):
	{
		FileManager *fmg = FileManager::GetInstance();
		value			 = fmg->readSettingFromFile(PieceTheme);
		LOG_INFO("Piece theme read from file : {}", value.c_str());
		return value;
	}
	case (SettingsType::piecesTheme):
	{
		FileManager *fmg = FileManager::GetInstance();
		value			 = fmg->readSettingFromFile(PlayerNameSetting);
		LOG_INFO("Player name read from file : {}", value.c_str());
		return value;
	}
	default: return value;
	}
}



bool UserSettings::doesConfigFileExist()
{
	FileManager *fmg			  = FileManager::GetInstance();
	fs::path	 userSettingsPath = fmg->getUserSettingsPath();
	return fs::exists(userSettingsPath);
}


void UserSettings::setCurrentBoardTheme(std::string theme)
{
	storeSetting(SettingsType::boardTheme, theme);
	LOG_INFO("Set the Board Theme to {}", theme.c_str());
}


std::string UserSettings::getCurrentBoardTheme()
{
	return readSetting(SettingsType::boardTheme);
}


void UserSettings::setCurrentPieceTheme(std::string theme)
{
	storeSetting(SettingsType::piecesTheme, theme);
	LOG_INFO("Set the Piece Theme to {}", theme.c_str());
}


std::string UserSettings::getCurrentPieceTheme()
{
	return readSetting(SettingsType::piecesTheme);
}


void UserSettings::setLocalPlayerName(std::string theme)
{
	storeSetting(SettingsType::piecesTheme, theme);
	LOG_INFO("Set the Piece Theme to {}", theme.c_str());
}


std::string UserSettings::getLocalPlayerName()
{
	return readSetting(SettingsType::playerName);
}


bool UserSettings::initializeConfigFile(DefaultSettings settings)
{
	storeSetting(SettingsType::boardTheme, settings.BoardTheme);
	storeSetting(SettingsType::piecesTheme, settings.PieceTheme);
}


void UserSettings::logUserSettings()
{
	const std::string piecesTheme = getCurrentPieceTheme();
	const std::string boardTheme  = getCurrentBoardTheme();
	const std::string playerName  = getLocalPlayerName();

	LOG_INFO("------------------ User Settings ------------------");
	LOG_INFO("Board Theme :\t{}", boardTheme);
	LOG_INFO("Board Theme :\t{}", piecesTheme);
	LOG_INFO("Player Name :\t{}", playerName);
	LOG_INFO("---------------------------------------------------");
}
