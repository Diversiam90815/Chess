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

	void joinSession(const std::string IPv4, const int port);

	void setTCPSession(TCPSession::pointer session);


private:
	bool					   presetNetworkAdapter();

	bool					   setNetworkAdapterFromConfig();


	TCPSession::pointer		   mSession = nullptr;

	std::unique_ptr<TCPServer> mServer;

	std::unique_ptr<TCPClient> mClient;

	NetworkInformation		   mNetworkInfo;
};
