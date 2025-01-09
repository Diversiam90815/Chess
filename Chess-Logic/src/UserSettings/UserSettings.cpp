/*
  ==============================================================================

	Class:          UserSetting

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
}


std::string UserSettings::readSetting(SettingsType setting)
{
	return std::string();
}


void UserSettings::initializeValues()
{
}


void UserSettings::initializeConfigFile()
{
}


bool UserSettings::doesConfigFileExist()
{
	return false;
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
