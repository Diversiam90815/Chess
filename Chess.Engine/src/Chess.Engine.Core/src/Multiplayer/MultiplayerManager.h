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
#include "PlayerName.h"

/**
 * @brief Coordinates multiplayer lifecycle: discovery, connection negotiation, TCP session
 *        management, player color selection, readiness synchronization, and remote message flow.
 */
class MultiplayerManager : public INetworkObserver,
						   public IConnectionStatusObservable,
						   public IDiscoveryObserver,
						   public IRemoteMessagesObserver,
						   public std::enable_shared_from_this<MultiplayerManager>
{
public:
	MultiplayerManager();
	~MultiplayerManager();

	/**
	 * @brief	Initialize manager with local network parameters.
	 * @param	localIPv4 -> Local machine IPv4 used for discovery and binding.
	 * @return	true if initialization succeeds.
	 */
	bool		init(const std::string &localIPv4);

	/**
	 * @brief	Reset internal state (disconnects, clears flags/endpoints).
	 */
	void		reset();

	/**
	 * @brief	Start hosting (create TCP server + discovery broadcast).
	 * @return	true if server successfully started.
	 */
	bool		hostSession();

	/**
	 * @brief	Start client mode (discovery to locate host, then request connection).
	 * @return	true if initial client startup succeeded.
	 */
	bool		startClient();

	/**
	 * @brief	Attempt to join a previously discovered remote host.
	 * @note	Assumes discovery has populated remote endpoint.
	 */
	void		joinSession();

	/**
	 * @brief	Assign established TCP session (after accept/connect).
	 */
	void		setTCPSession(ITCPSession::pointer session);

	/**
	 * @brief	Disconnect current session and tear down communication channels.
	 */
	void		disconnect();

	/**
	 * @brief	Local player's name (forwarded from PlayerName helper).
	 */
	std::string getLocalPlayerName() { return mPlayerName.getLocalPlayerName(); }

	/**
	 * @brief	Notified when active network adapter changes (re-evaluate discovery sockets).
	 */
	void		onNetworkAdapterChanged(const NetworkAdapter &adapter) override;

	/**
	 * @brief	Begin discovery in specified mode (host advertising or client searching).
	 * @param	IPv4 -> Local IPv4.
	 * @param	port -> TCP port host intends to use.
	 * @param	mode -> Discovery mode (host/client).
	 * @return	true if discovery started.
	 */
	bool		startDiscovery(const std::string IPv4, const int port, DiscoveryMode mode);

	/**
	 * @brief	Wire internal observer relationships (sender/receiver/session).
	 */
	void		setInternalObservers();

	/**
	 * @brief	Send connection request to host (client side).
	 */
	void		sendConnectRequest();

	/**
	 * @brief	Send connection response (host side) accepting or rejecting.
	 * @param	accepted -> True if accepted.
	 * @param	reason -> Optional rejection reason.
	 */
	void		sendConnectResponse(bool accepted, std::string reason = "");

	/**
	 * @brief	Observer callback when connection status changes (propagate outward).
	 */
	void		connectionStatusChanged(const ConnectionStatusEvent event) override;

	/**
	 * @brief	Local player color decision.
	 */
	void		localPlayerChosen(const Side localPlayer) override;

	/**
	 * @brief	Remote player has chosen his/her color. Observer contract.
	 * @param	localPlayer -> already converted remote's color to the local player (Remote selects white, local is black,..).
	 */
	void		remotePlayerChosen(const Side local) override;

	/**
	 * @brief	Local ready flag toggled (used for synchronized game start). Observer contract.
	 */
	void		localReadyFlagSet(const bool flag) override;

	/**
	 * @brief	Discovery service found a remote endpoint. Observer contract.
	 * @param	remote -> remote endpoint that has been found on the local network
	 */
	void		onRemoteFound(const Endpoint &remote) override;

	/**
	 * @brief	We received a move from the remote endpoint
	 */
	void		onRemoteMoveReceived(const Move &remoteMove) override;

	void		onRemoteChatMessageReceived(const std::string &mesage) override {}

	/**
	 * @brief	Remote connection state broadcast received. Observer contract.
	 */
	void		onRemoteConnectionStateReceived(const ConnectionState &state) override;

	/**
	 * @brief	Invitation request received from remote (client -> host). Observer contract.
	 */
	void		onRemoteInvitationReceived(const InvitationRequest &invite) override;

	/**
	 * @brief	Response to earlier invitation (host -> client). Observer contract.
	 */
	void		onRemoteInvitationResponseReceived(const InvitationResponse &response) override;

	/**
	 * @brief	Remote player's color selection arrived. Observer contract.
	 */
	void		onRemotePlayerChosenReceived(const Side player) override;

	/**
	 * @brief	Remote player's readiness flag changed. Observer contract.
	 * @param	flag -> true if the remote is ready to start the multiplayer game
	 */
	void		onRemotePlayerReadyFlagReceived(const bool flag) override;

	/**
	 * @brief	Set remote ready flag (internal update without event callback loop).
	 * @param	flag -> true if the remote is ready to start the multiplayer game
	 */
	void		setRemotePlayerReadyForGameFlag(const bool flag);

	/**
	 * @brief	Check if both local and remote players are ready to start.
	 * @return	true if ready to transition into active game state.
	 */
	bool		checkIfReadyForGame();

	void		setRemoteMoveCallback(std::function<void(Move)> callback);

private:
	/**
	 * @brief	Stop discovery service if running.
	 */
	void													   closeDiscovery();

	/**
	 * @brief	Shutdown TCP server or client (whichever active).
	 */
	void													   closeTCPServerOrClient();

	/**
	 * @brief	Tear down remote communication abstraction (sender/receiver linkage).
	 */
	void													   closeRemoteCommunication();


	ITCPSession::pointer									   mSession = nullptr;

	std::unique_ptr<TCPServer>								   mServer;
	std::unique_ptr<TCPClient>								   mClient;
	std::unique_ptr<DiscoveryService>						   mDiscovery;

	std::shared_ptr<RemoteCommunication>					   mRemoteCom;
	std::shared_ptr<RemoteReceiver>							   mRemoteReceiver;
	std::shared_ptr<RemoteSender>							   mRemoteSender;

	asio::io_context										   mIoContext;
	asio::executor_work_guard<asio::io_context::executor_type> mWorkGuard;
	std::thread												   mWorkerThread;

	Side													   mLocalPlayerColor{};
	std::string												   mLocalIPv4{};

	Endpoint												   mRemoteEndpoint;

	std::mutex												   mSessionMutex;

	ConnectionStatusEvent									   mConnectionState{ConnectionState::None};
	ConnectionStatusEvent									   mRemoteConnectionState{ConnectionState::None};

	std::atomic<bool>										   mLocalPlayerReadyForGameFlag{false};
	std::atomic<bool>										   mRemotePlayerReadyForGameFlag{false};

	PlayerName												   mPlayerName;

	std::function<void(Move)>								   mRemoteMoveReceivedCallback;

	friend class GameManager;
};
