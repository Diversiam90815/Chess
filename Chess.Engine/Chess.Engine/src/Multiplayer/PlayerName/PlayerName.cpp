/*
  ==============================================================================
	Module:         PlayerName
	Description:    Managing the local player's PC name for multiplayer
  ==============================================================================
*/

#include "PlayerName.h"


void PlayerName::init() {}


void PlayerName::setLocalPlayerName(const std::string &name){mLocalPlayerName = name;
}


std::string PlayerName::getLocalPlayerName()
{
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

