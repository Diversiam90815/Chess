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


class NetworkManager
{
public:
	NetworkManager();
	~NetworkManager();

	void init();

	void hostSession();

	void joinSession();


private:
	bool					   presetNetworkAdapter();

	bool					   setNetworkAdapterFromConfig();


	TCPSession::pointer		   mSession = nullptr;

	std::unique_ptr<TCPServer> mServer;

	std::unique_ptr<TCPClient> mClient;

	NetworkInformation		   mNetworkInfo;
};
