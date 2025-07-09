/*
  ==============================================================================
	Module:         UserSetting
	Description:    Storing and managing the settings set by the user
  ==============================================================================
*/

#pragma once

#include "FileManager.h"
#include "Logging.h"
#include "Parameters.h"

struct DefaultSettings
{
	const std::string BoardTheme = "Wood";
	const std::string PieceTheme = "Basic";
};


class UserSettings
{
public:
	UserSettings()	= default;
	~UserSettings() = default;

	void		init();

	void		storeSetting(SettingsType setting, std::string value);

	std::string readSetting(SettingsType setting);


	void		setCurrentBoardTheme(std::string theme);
	std::string getCurrentBoardTheme();

	void		setCurrentPieceTheme(std::string theme);
	std::string getCurrentPieceTheme();

	void		setLocalPlayerName(const std::string name);
	std::string getLocalPlayerName();


private:
	bool			doesConfigFileExist();

	bool			initializeConfigFile(DefaultSettings settings);

	void			logUserSettings();

	DefaultSettings mDefaultSettings;
};
