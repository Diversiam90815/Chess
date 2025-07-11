/*
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
#include "UserSettings.h"


class MultiplayerManager : public INetworkObserver,
						   public IConnectionStatusObservable,
						   public IDiscoveryObserver,
						   public IRemoteMessagesObserver,
						   public std::enable_shared_from_this<MultiplayerManager>
{
public:
	MultiplayerManager();
	~MultiplayerManager();

	void		init(const std::string &localIPv4);

	void		reset();

	bool		hostSession();
	bool		startClient();
	void		joinSession();

	void		setTCPSession(TCPSession::pointer session);

	void		disconnect();

	std::string getLocalPlayerName() { return mLocalPlayerName; }

	void		onNetworkAdapterChanged(const NetworkAdapter &adapter) override;

	bool		startDiscovery(const std::string IPv4, const int port, DiscoveryMode mode);

	void		setInternalObservers();

	void		sendConnectRequest();
	void		sendConnectResponse(bool accepted, std::string reason = "");

	void		connectionStatusChanged(const ConnectionStatusEvent event) override;
	void		localPlayerChosen(const PlayerColor localPlayer) override;
	void		remotePlayerChosen(const PlayerColor local) override; // This is the already set to the localPlayer, but set if the remote chose a player
	void		localReadyFlagSet(const bool flag) override;

	void		onRemoteFound(const Endpoint &remote) override;

	void		onRemoteMoveReceived(const PossibleMove &remoteMove) override {}
	void		onRemoteChatMessageReceived(const std::string &mesage) override {}
	void		onRemoteConnectionStateReceived(const ConnectionState &state) override;
	void		onRemoteInvitationReceived(const InvitationRequest &invite) override;
	void		onRemoteInvitationResponseReceived(const InvitationResponse &response) override;
	void		onRemotePlayerChosenReceived(const PlayerColor player) override;
	void		onRemotePlayerReadyFlagReceived(const bool flag) override;

	void		setRemotePlayerReadyForGameFlag(const bool flag);
	bool		checkIfReadyForGame();


private:
	void													   closeDiscovery();
	void													   closeTCPServerOrClient();
	void													   closeRemoteCommunication();


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

	PlayerColor												   mLocalPlayerColor{};
	std::string												   mLocalPlayerName;
	std::string												   mLocalIPv4{};

	Endpoint												   mRemoteEndpoint;

	std::mutex												   mSessionMutex;

	ConnectionStatusEvent									   mConnectionState{ConnectionState::None};
	ConnectionStatusEvent									   mRemoteConnectionState{ConnectionState::None};

	std::atomic<bool>										   mLocalPlayerReadyForGameFlag{false};
	std::atomic<bool>										   mRemotePlayerReadyForGameFlag{false};

	UserSettings											   mUserSettings;

	friend class GameManager;
};
