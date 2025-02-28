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

	void						init();

	bool						hostSession();

	void						joinSession(const Endpoint remote);

	void						setTCPSession(TCPSession::pointer session);

	std::vector<NetworkAdapter> getAvailableNetworkAdapters() const;
	bool						changeNetworkAdapter(const int ID);

	int							getCurrentNetworkAdapterID();

	bool						isInitialized() const { return initialized.load(); }
	void						setInitialized(const bool value) { initialized.store(value); }

	void						setLocalPlayerName(const std::string name) { mLocalPlayerName = name; }
	std::string					getLocalPlayerName() const { return mLocalPlayerName; }

	void						setRemotePlayerName(const std::string name) { mRemotePlayerName = name; }
	std::string					getRemotePlayerName() const { return mRemotePlayerName; }


private:
	bool																	 presetNetworkAdapter();

	bool																	 setNetworkAdapterFromConfig();

	bool																	 startServerDiscovery(const std::string IPv4, const int port);
	void																	 startClientDiscovery();


	TCPSession::pointer														 mSession = nullptr;

	std::unique_ptr<TCPServer>												 mServer;
	std::unique_ptr<TCPClient>												 mClient;
	std::unique_ptr<DiscoveryService>										 mDiscovery;

	NetworkInformation														 mNetworkInfo;

	std::atomic<bool>														 initialized{false};

	boost::asio::io_context													 mIoContext;
	boost::asio::executor_work_guard<boost::asio::io_context::executor_type> mWorkGuard;
	std::thread																 mWorkerThread;

	std::string																 mLocalPlayerName{};
	std::string																 mRemotePlayerName{};
};
