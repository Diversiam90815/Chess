/*
  ==============================================================================

	Class:          NetworkManager

	Description:    Managing the network part of this app

  ==============================================================================
*/

#pragma once

#include "NetworkInformation.h"
#include "FileManager.h"


class NetworkManager
{
public:
	NetworkManager();
	~NetworkManager();

	void init();


private:
	bool			   presetNetworkAdapter();

	bool			   setNetworkAdapterFromConfig();


	NetworkInformation mNetworkInfo;
};
