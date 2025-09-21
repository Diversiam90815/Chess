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

	initializeNetworkAdapter();

	setInitialized(true);
}


const std::vector<NetworkAdapter> &NetworkManager::getAvailableNetworkAdapters() const
{
	return mNetworkInfo.getAvailableNetworkAdapters();
}


void NetworkManager::networkAdapterChanged(const NetworkAdapter &adapter)
{
	auto &currentAdapter = mNetworkInfo.getCurrentNetworkAdapter();

	if (currentAdapter != adapter)
	{
		LOG_INFO("Network Adapter has been changed to {} with IP", adapter.AdapterName.c_str(), adapter.IPv4.c_str());

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


const std::string &NetworkManager::getCurrentIPv4()
{
	return mNetworkInfo.getCurrentNetworkAdapter().IPv4;
}


bool NetworkManager::changeCurrentNetworkAdapter(const int ID)
{
	auto &adapters = getAvailableNetworkAdapters();

	for (auto &adapter : adapters)
	{
		if (adapter.ID != ID)
			continue;

		networkAdapterChanged(adapter);
		return true;
	}
	return false;
}


void NetworkManager::initializeNetworkAdapter()
{
	auto		   adapterFromConfig		= mUserSettings.getNetworkAdapter();
	bool		   adapterFromConfigIsValid = adapterFromConfig.isValid();

	// Let's see if the adapter found in the config is currently available
	NetworkAdapter currentConfigAdapter		= mNetworkInfo.isAdapterCurrentlyAvailable(adapterFromConfig);
	bool		   isAvailable				= currentConfigAdapter.isValid(); // If the adapter exists, we receive the current version of the adapter, else an empty adapter

	if (!adapterFromConfigIsValid)
		LOG_INFO("Adapter from config is not valid!");

	if (!isAvailable)
		LOG_INFO("Adapter from config is currently not available!");

	if (adapterFromConfigIsValid && isAvailable)
		networkAdapterChanged(currentConfigAdapter);
	else
		initAdapterInConfig();
}


void NetworkManager::initAdapterInConfig()
{
	auto &adapters = mNetworkInfo.getAvailableNetworkAdapters();

	LOG_INFO("Filtering from {} adapters for new standard network adapter", adapters.size());

	for (auto &adapter : adapters)
	{
		if ((adapter.IsDefaultRoute) && (adapter.Type == AdapterTypes::Ethernet || adapter.Type == AdapterTypes::WiFi))
		{
			networkAdapterChanged(adapter);
			return;
		}
	}

	LOG_INFO("Could not find adapter fulfilling first set of settings. Trying a more relaxed search!");

	for (auto &adapter : adapters)
	{
		if (adapter.IsDefaultRoute && (adapter.Type != AdapterTypes::Loopback && adapter.Type != AdapterTypes::Virtual))
		{
			networkAdapterChanged(adapter);
			return;
		}
	}

	LOG_WARNING("Could not find an adapter fulfilling the set of settings.. Falling back to eligibility setting via subnet mask!");

	for (auto &adapter : adapters)
	{
		if (adapter.eligible)
		{
			networkAdapterChanged(adapter);
			return;
		}
	}

	LOG_WARNING("Could not find eligible adapter with proper subnet mask. We are forced to pick the first adapter in list now..");

	if (adapters.size() > 0)
	{
		networkAdapterChanged(adapters[0]);
		return;
	}

	LOG_ERROR("Could not find any adapter to select as default! No adapters available!");
}
