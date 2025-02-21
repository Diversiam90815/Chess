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

	if (configFileExists)
	{
		LOG_INFO("User Config found");
		initializeValues();
		return;
	}
	LOG_INFO("User Config not found, so we set up one!");
	initializeConfigFile();
}


void UserSettings::storeSetting(SettingsType setting, std::string value)
{
	switch (setting)
	{
	case (SettingsType::boardTheme):
	{
		LOG_INFO("Board theme setting set {}", value.c_str());
		FileManager *fmg = FileManager::GetInstance();
		fmg->writeSettingToFile(BoardTheme, value);
		break;
	}
	case (SettingsType::piecesTheme):
	{
		LOG_INFO("Piece theme setting set {}", value.c_str());
		FileManager *fmg = FileManager::GetInstance();
		fmg->writeSettingToFile(PieceTheme, value);
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
	default: break;
	}
}


void UserSettings::initializeValues()
{
	mCurrentBoardTheme = readSetting(SettingsType::boardTheme);
	mCurrentPieceTheme = readSetting(SettingsType::piecesTheme);
}


void UserSettings::initializeConfigFile()
{
	storeSetting(SettingsType::boardTheme, mCurrentBoardTheme);
	storeSetting(SettingsType::piecesTheme, mCurrentPieceTheme);
}


bool UserSettings::doesConfigFileExist()
{
	FileManager *fmg			  = FileManager::GetInstance();
	fs::path	 userSettingsPath = fmg->getUserSettingsPath();
	return fs::exists(userSettingsPath);
}


void UserSettings::setCurrentBoardTheme(std::string theme)
{
	if (mCurrentBoardTheme != theme)
	{
		mCurrentBoardTheme = theme;
		storeSetting(SettingsType::boardTheme, theme);
		LOG_INFO("Set the Board Theme to {}", theme.c_str());
	}
}


std::string UserSettings::getCurrentBoardTheme() const
{
	return mCurrentBoardTheme;
}


void UserSettings::setCurrentPieceTheme(std::string theme)
{
	if (mCurrentPieceTheme != theme)
	{
		mCurrentPieceTheme = theme;
		storeSetting(SettingsType::piecesTheme, theme);
		LOG_INFO("Set the Piece Theme to {}", theme.c_str());
	}
}


std::string UserSettings::getCurrentPieceTheme() const
{
	return mCurrentPieceTheme;
}
