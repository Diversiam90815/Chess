/*
  ==============================================================================
	Module:         NetworkManager
	Description:    Managing the network part of this app
  ==============================================================================
*/

#pragma once

#include "NetworkInformation.h"
#include "FileManager.h"
#include "IObservable.h"

class NetworkManager : public INetworkObservable
{
public:
	NetworkManager()  = default;
	~NetworkManager() = default;

	void						init();

	std::vector<NetworkAdapter> getAvailableNetworkAdapters() const;
	void						networkAdapterChanged(const NetworkAdapter &adapter) override;

	int							getCurrentNetworkAdapterID();

	bool						isInitialized() const { return initialized.load(); }
	void						setInitialized(const bool value) { initialized.store(value); }

	std::string					getCurrentIPv4();
	bool						changeCurrentNetworkAdapter(const int ID);

private:
	bool			   presetNetworkAdapter();

	bool			   setNetworkAdapterFromConfig();

	NetworkInformation mNetworkInfo;

	std::atomic<bool>  initialized{false};
};
