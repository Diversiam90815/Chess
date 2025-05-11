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


class MultiplayerManager : public INetworkObserver, public IConnectionStatusObservable, public IDiscoveryObserver
{
public:
	MultiplayerManager();
	~MultiplayerManager();

	bool				hostSession();

	void				joinSession();

	void				setTCPSession(TCPSession::pointer session);
	TCPSession::pointer getActiveSession();

	void				disconnect();

	void				setLocalPlayerName(const std::string name) { mLocalPlayerName = name; }
	std::string			getLocalPlayerName() const { return mLocalPlayerName; }

	void				onNetworkAdapterChanged(const NetworkAdapter &adapter) override;

	bool				startServerDiscovery(const std::string IPv4, const int port);
	void				startClientDiscovery();

	void				setInternalObservers();

	void				approveConnectionRequest();
	void				rejectConnectionRequest();

	void				connectionStatusChanged(ConnectionState state, const std::string &errorMessage = "") override;
	void				pendingHostApproval(const std::string &remoteIPv4) override;

	void				onRemoteFound(const Endpoint &remote) override;

private:
	TCPSession::pointer										   mSession = nullptr;

	std::unique_ptr<TCPServer>								   mServer;
	std::unique_ptr<TCPClient>								   mClient;
	std::unique_ptr<DiscoveryService>						   mDiscovery;

	std::shared_ptr<RemoteCommunication>					   mRemoteCom;
	std::shared_ptr<RemoteReceiver>							   mRemoteReceiver;
	std::shared_ptr<RemoteSender>							   mRemoteSender;

	asio::io_context										   mIoContext;
	asio::executor_work_guard<asio::io_context::executor_type> mWorkGuard;
	std::thread												   mWorkerThread;

	std::string												   mLocalPlayerName{};

	std::string												   mLocalIPv4{};

	Endpoint												   mRemoteEndpoint;

	std::mutex												   mSessionMutex;

	std::atomic<ConnectionState>							   mConnectionState{ConnectionState::Disconnected};

	friend class GameManager;
};
