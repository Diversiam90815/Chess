/*
  ==============================================================================
	Module:         PlayerName
	Description:    Managing the local player's PC name for multiplayer
  ==============================================================================
*/

#pragma once

#include <Windows.h>

#include "UserSettings.h"
#include "Logging.h"

/// <summary>
/// Represents and manages the local player's name, including setting and retrieving it.
/// </summary>
class PlayerName
{
public:
	PlayerName()  = default;
	~PlayerName() = default;

	void		setLocalPlayerName(const std::string &name);
	std::string getLocalPlayerName();

private:
	// At first startup we select the local PC name as the player name by default
	std::string	 getComputerNameAsPlayerName();


	std::string	 mLocalPlayerName;

	UserSettings mUserSettings;
};
