/*
  ==============================================================================
	Module:         PlayerName
	Description:    Managing the local player's PC name for multiplayer
  ==============================================================================
*/

#include "PlayerName.h"


void PlayerName::setLocalPlayerName(const std::string &name)
{
	if (mLocalPlayerName == name)
		return;

	mLocalPlayerName = name;
	mUserSettings.setLocalPlayerName(name);

	LOG_INFO("Local Player name has been set to : {}", name);
}


std::string PlayerName::getLocalPlayerName()
{
	if (!mLocalPlayerName.empty())
		return mLocalPlayerName;

	std::string tmpName = mUserSettings.getLocalPlayerName();

	if (tmpName.empty())
	{
		// If the name does not exist currently (eg. at first startup), we select this PC's name
		// There is no name set in the config
		tmpName = getComputerNameAsPlayerName();
	}

	setLocalPlayerName(tmpName);

	return mLocalPlayerName;
}


std::string PlayerName::getComputerNameAsPlayerName()
{
	DWORD bufferSize = MAX_COMPUTERNAME_LENGTH + 1;
	char  buffer[MAX_COMPUTERNAME_LENGTH + 1];

	if (GetComputerNameA(buffer, &bufferSize))
	{
		return std::string(buffer, bufferSize);
	}
	else
	{
		return "Unknown";
	}
}
