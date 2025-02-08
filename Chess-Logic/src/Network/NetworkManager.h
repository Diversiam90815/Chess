/*
  ==============================================================================

	Class:          NetworkManager

	Description:    Managing the network part of this app

  ==============================================================================
*/

#pragma once

#include "NetworkInformation.h"


class NetworkManager
{
public:
	NetworkManager();
	~NetworkManager();

	void init();


private:

	NetworkInformation mNetworkInfo;
};
