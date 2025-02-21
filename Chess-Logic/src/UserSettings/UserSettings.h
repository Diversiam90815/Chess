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

class UserSettings
{
public:
	UserSettings()	= default;
	~UserSettings() = default;

	void		init();

	void		storeSetting(SettingsType setting, std::string value);

	std::string readSetting(SettingsType setting);

	void		initializeValues();
	void		initializeConfigFile();

	bool		doesConfigFileExist();

	void		setCurrentBoardTheme(std::string theme);
	std::string getCurrentBoardTheme() const;

	void		setCurrentPieceTheme(std::string theme);
	std::string getCurrentPieceTheme() const;


private:
	std::string mCurrentBoardTheme = "Wood";
				 
	std::string mCurrentPieceTheme = "Basic";

	bool		mInitialized	   = false;
};
