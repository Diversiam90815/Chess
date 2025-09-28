/*
  ==============================================================================
	Module:         NetworkManager
	Description:    Managing the network part of this app
  ==============================================================================
*/

#pragma once

#include "NetworkInformation.h"
#include "IObservable.h"
#include "UserSettings.h"

/// <summary>
/// Manages network adapters and provides network-related operations, including initialization, adapter selection, and status queries.
/// </summary>
class NetworkManager : public INetworkObservable
{
public:
	NetworkManager()  = default;
	~NetworkManager() = default;

	void							   init();

	const std::vector<NetworkAdapter> &getAvailableNetworkAdapters() const;
	void							   networkAdapterChanged(const NetworkAdapter &adapter) override;

	int								   getCurrentNetworkAdapterID();

	bool							   isInitialized() const { return initialized.load(); }
	void							   setInitialized(const bool value) { initialized.store(value); }

	const std::string				  &getCurrentIPv4();
	bool							   changeCurrentNetworkAdapter(const int ID);

private:
	void			   initializeNetworkAdapter();

	void			   initAdapterInConfig();

	UserSettings	   mUserSettings;

	NetworkInformation mNetworkInfo;

	std::atomic<bool>  initialized{false};
};
