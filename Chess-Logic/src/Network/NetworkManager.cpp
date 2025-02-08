/*
  ==============================================================================

	Class:          NetworkManager

	Description:    Managing the network part of this app

  ==============================================================================
*/

#include "NetworkManager.h"


NetworkManager::NetworkManager() {}


NetworkManager::~NetworkManager() {}


void NetworkManager::init()
{
	mNetworkInfo.init();

	mNetworkInfo.processAdapter();
}
