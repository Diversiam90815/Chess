/*
  ==============================================================================
	Module:         NetworkManager
	Description:    Managing the network part of this app
  ==============================================================================
*/

#include "NetworkManager.h"


void NetworkManager::init()
{
	mNetworkInfo.init();

	mNetworkInfo.processAdapter();

	if (!setNetworkAdapterFromConfig()) // Will return false if we find an adapter in config file
	{
		presetNetworkAdapter();
	}

	setInitialized(true);
}


std::vector<NetworkAdapter> NetworkManager::getAvailableNetworkAdapters() const
{
	return mNetworkInfo.getAvailableNetworkAdapters();
}


void NetworkManager::networkAdapterChanged(const NetworkAdapter &adapter)
{
	auto currentAdapter = mNetworkInfo.getCurrentNetworkAdapter();

	if (currentAdapter != adapter)
	{
		LOG_INFO("Network Adapter has been changed to {} with IP", adapter.description.c_str(), adapter.IPv4.c_str());

		mNetworkInfo.setCurrentNetworkAdapter(adapter);

		for (auto &observer : mObservers)
		{
			if (auto obs = observer.lock())
				obs->onNetworkAdapterChanged(adapter);
		}
	}
}


int NetworkManager::getCurrentNetworkAdapterID()
{
	return mNetworkInfo.getCurrentNetworkAdapter().ID;
}


std::string NetworkManager::getCurrentIPv4()
{
	return mNetworkInfo.getCurrentNetworkAdapter().IPv4;
}


bool NetworkManager::changeCurrentNetworkAdapter(const int ID)
{
	auto adapters = getAvailableNetworkAdapters();

	for (auto &adapter : adapters)
	{
		if (adapter.ID != ID)
			continue;

		networkAdapterChanged(adapter);
		return true;
	}
	return false;
}


bool NetworkManager::presetNetworkAdapter()
{
	auto adapterInFile = FileManager::GetInstance()->readSelectedNetworkAdapter();

	if (adapterInFile.has_value())
		return false; // No need to preset if we have stored a selected adapter

	LOG_INFO("Since no adapter could been found in the config file, we are selecting one!");

	auto adapter = mNetworkInfo.getFirstEligibleAdapter();

	networkAdapterChanged(adapter);

	return true;
}


bool NetworkManager::setNetworkAdapterFromConfig()
{
	auto adapterInFile = FileManager::GetInstance()->readSelectedNetworkAdapter();

	if (!adapterInFile.has_value())
		return false;

	LOG_INFO("Found a network adapter in the config file!");

	auto &userSetAdapter = adapterInFile.value();

	if (!mNetworkInfo.isAdapterCurrentlyAvailable(userSetAdapter))
		return false;

	networkAdapterChanged(userSetAdapter);

	return true;
}
