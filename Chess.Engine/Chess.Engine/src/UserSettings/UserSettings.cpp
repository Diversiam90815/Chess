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


bool UserSettings::doesConfigFileExist()
{
	FileManager *fmg			  = FileManager::GetInstance();
	fs::path	 userSettingsPath = fmg->getUserSettingsPath();
	return fs::exists(userSettingsPath);
}


void UserSettings::setCurrentBoardTheme(std::string theme)
{
	storeSetting(SettingsType::BoardStyle, theme);
	LOG_INFO("Set the Board Style to {}", theme.c_str());
}


std::string UserSettings::getCurrentBoardTheme()
{
	return readSetting<std::string>(SettingsType::BoardStyle);
}


void UserSettings::setCurrentPieceTheme(std::string theme)
{
	storeSetting(SettingsType::ChessPieceStyle, theme);
	LOG_INFO("Set the Piece Style to {}", theme.c_str());
}


std::string UserSettings::getCurrentPieceTheme()
{
	return readSetting<std::string>(SettingsType::ChessPieceStyle);
}


void UserSettings::setLocalPlayerName(std::string name)
{
	storeSetting(SettingsType::PlayerName, name);
	LOG_INFO("Set the ChessPiece Style to {}", name.c_str());
}


std::string UserSettings::getLocalPlayerName()
{
	return readSetting<std::string>(SettingsType::PlayerName);
}


void UserSettings::setSFXEnabled(const bool enabled)
{
	storeSetting<bool>(SettingsType::AudioSFXEnabled, enabled);
	LOG_INFO("Set SFX enabled to {}", LoggingHelper::boolToString(enabled).c_str());
}


bool UserSettings::getSFXEnabled()
{
	return readSetting<bool>(SettingsType::AudioSFXEnabled);
}


void UserSettings::setAtmosEnabled(const bool enabled)
{
	storeSetting<bool>(SettingsType::AudioAtmosEnabled, enabled);
	LOG_INFO("Set Atmos enabled to {}", LoggingHelper::boolToString(enabled).c_str());
}


bool UserSettings::getAtmosEnabled()
{
	return readSetting<bool>(SettingsType::AudioAtmosEnabled);
}


void UserSettings::setSFXVolume(const float volume)
{
	storeSetting<float>(SettingsType::AudioSFXVolume, volume);
	LOG_INFO("Set SFX volume to {}", volume);
}


float UserSettings::getSFXVolume()
{
	return readSetting<float>(SettingsType::AudioSFXVolume);
}


void UserSettings::setAtmosVolume(const float volume)
{
	storeSetting<float>(SettingsType::AudioAtmosVolume, volume);
	LOG_INFO("Set Atmos volume to {}", volume);
}


float UserSettings::getAtmosVolume()
{
	return readSetting<float>(SettingsType::AudioAtmosVolume);
}


void UserSettings::setMasterAudioVolume(const float volume)
{
	storeSetting<float>(SettingsType::AudioMasterVolume, volume);
	LOG_INFO("Set master volume to {}", volume);
}


float UserSettings::getMasterVolume()
{
	return readSetting<float>(SettingsType::AudioMasterVolume);
}


void UserSettings::setAtmosScenario(const std::string scenario)
{
	storeSetting<std::string>(SettingsType::AudioAtmosScenario, scenario);
	LOG_INFO("Set atmos scenario to {}", scenario.c_str());
}


std::string UserSettings::getAtmosScenario()
{
	return readSetting<std::string>(SettingsType::AudioAtmosScenario);
}


void UserSettings::initializeConfigFile(DefaultSettings settings)
{
	storeSetting(SettingsType::BoardStyle, settings.BoardStyle);
	storeSetting(SettingsType::ChessPieceStyle, settings.ChessPieceStyle);
}


void UserSettings::logUserSettings()
{
	const std::string piecesTheme = getCurrentPieceTheme();
	const std::string boardTheme  = getCurrentBoardTheme();
	const std::string playerName  = getLocalPlayerName();

	LOG_INFO("------------------ User Settings ------------------");
	LOG_INFO("Board Style :\t{}", boardTheme);
	LOG_INFO("Board Style :\t{}", piecesTheme);
	LOG_INFO("Player Name :\t{}", playerName);
	LOG_INFO("---------------------------------------------------");
}
