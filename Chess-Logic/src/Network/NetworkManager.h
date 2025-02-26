/*
  ==============================================================================
	Module:         NetworkManager
	Description:    Managing the network part of this app
  ==============================================================================
*/

#pragma once

#include "NetworkInformation.h"
#include "FileManager.h"
#include "TCPClient.h"
#include "TCPServer.h"
#include "DiscoveryService.h"


class NetworkManager
{
public:
	NetworkManager();
	~NetworkManager();

	void init();

	bool hostSession();

	void joinSession(const std::string IPv4, const int port);

	void setTCPSession(TCPSession::pointer session);

	bool isInitialized() const { return initialized.load(); }
	void setInitialized(const bool value) { initialized.store(value); }


private:
	bool							  presetNetworkAdapter();

	bool							  setNetworkAdapterFromConfig();

	void							  startDiscovery(const std::string IPv4, const int port);


	TCPSession::pointer				  mSession = nullptr;

	std::unique_ptr<TCPServer>		  mServer;

	std::unique_ptr<TCPClient>		  mClient;

	std::unique_ptr<DiscoveryService> mDiscovery;

	NetworkInformation				  mNetworkInfo;

	std::atomic<bool>				  initialized{false};
};
