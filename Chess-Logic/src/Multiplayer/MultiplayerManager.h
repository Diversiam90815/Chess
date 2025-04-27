/*
  ==============================================================================
	Module:         MultiplayerManager
	Description:    Managing the multiplayer game mode
  ==============================================================================
*/

#pragma once

#include "TCPConnection/TCPClient.h"
#include "TCPConnection/TCPServer.h"
#include "Discovery/DiscoveryService.h"
#include "RemoteMessaging/RemoteReceiver.h"
#include "RemoteMessaging/RemoteSender.h"
#include "RemoteMessaging/RemoteCommunication.h"
#include "IObserver.h"


class MultiplayerManager : public INetworkObserver
{
public:
	MultiplayerManager();
	~MultiplayerManager();

	bool				hostSession();

	void				joinSession(const Endpoint remote);

	void				setTCPSession(TCPSession::pointer session);
	TCPSession::pointer getActiveSession();

	void				disconnect();

	void				setLocalPlayerName(const std::string name) { mLocalPlayerName = name; }
	std::string			getLocalPlayerName() const { return mLocalPlayerName; }

	void				setRemotePlayerName(const std::string name) { mRemotePlayerName = name; }
	std::string			getRemotePlayerName() const { return mRemotePlayerName; }

	void				onNetworkAdapterChanged(const NetworkAdapter &adapter) override;

	bool				startServerDiscovery(const std::string IPv4, const int port);
	void				startClientDiscovery();

	void				setInternalObservers();

private:
	TCPSession::pointer														 mSession = nullptr;

	std::unique_ptr<TCPServer>												 mServer;
	std::unique_ptr<TCPClient>												 mClient;
	std::unique_ptr<DiscoveryService>										 mDiscovery;

	std::shared_ptr<RemoteCommunication>									 mRemoteCom;
	std::shared_ptr<RemoteReceiver>											 mRemoteReceiver;
	std::shared_ptr<RemoteSender>											 mRemoteSender;

	boost::asio::io_context													 mIoContext;
	boost::asio::executor_work_guard<boost::asio::io_context::executor_type> mWorkGuard;
	std::thread																 mWorkerThread;

	std::string																 mLocalPlayerName{};
	std::string																 mRemotePlayerName{};

	std::string																 mLocalIPv4{};

	friend class GameManager;
};
