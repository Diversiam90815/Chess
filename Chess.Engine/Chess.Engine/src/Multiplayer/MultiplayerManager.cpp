/*
  ==============================================================================
	Module:         MultiplayerManager
	Description:    Managing the multiplayer game mode
  ==============================================================================
*/

#include "MultiplayerManager.h"



MultiplayerManager::MultiplayerManager() : mWorkGuard(asio::make_work_guard(mIoContext))
{
	// Start the IO Context in a dedicated thread
	mWorkerThread	= std::thread([this]() { mIoContext.run(); });

	mRemoteCom		= std::make_shared<RemoteCommunication>();
	mRemoteReceiver = std::make_shared<RemoteReceiver>();
	mRemoteSender	= std::make_shared<RemoteSender>();
}


MultiplayerManager::~MultiplayerManager()
{
	disconnect();

	// Stop the IO context
	mWorkGuard.reset();
	mIoContext.stop();

	if (mWorkerThread.joinable())
	{
		mWorkerThread.join();
	}
}


bool MultiplayerManager::init(const std::string &localIPv4)
{
	if (localIPv4.empty())
		return false;

	mLocalIPv4 = localIPv4;
	return true;
}


void MultiplayerManager::reset()
{
	// Stop discovery if running
	if (mDiscovery)
		mDiscovery->stop();

	// TODO: Reset server / client
}


bool MultiplayerManager::hostSession()
{
	if (mLocalIPv4.empty())
		return false;

	LOG_INFO("Hosting a session..");

	mServer = std::make_unique<TCPServer>(mIoContext);

	mServer->setSessionHandler([this](ITCPSession::pointer session) { setTCPSession(session); });

	mServer->startAccept();

	const int port	  = mServer->getBoundPort();
	bool	  success = startDiscovery(mLocalIPv4, port, DiscoveryMode::Server);

	return success;
}


bool MultiplayerManager::startClient()
{
	bool success = startDiscovery(mLocalIPv4, 0, DiscoveryMode::Client);
	return success;
}


void MultiplayerManager::joinSession()
{
	if (!mRemoteEndpoint.isValid())
		return;

	LOG_INFO("Joining session..");

	connectionStatusChanged(ConnectionState::PendingHostApproval);

	mClient = std::make_unique<TCPClient>(mIoContext);

	mClient->setConnectHandler(
		[this](ITCPSession::pointer session)
		{
			setTCPSession(session);
			sendConnectRequest();
		});

	mClient->setConnectTimeoutHandler([this]() { connectionStatusChanged({ConnectionState::Error, "Connection request timed out or was rejected!"}); });

	mClient->connect(mRemoteEndpoint.IPAddress, mRemoteEndpoint.tcpPort);
}


void MultiplayerManager::setTCPSession(ITCPSession::pointer session)
{
	{
		std::lock_guard<std::mutex> lock(mSessionMutex);
		mSession = session;
	}

	mRemoteCom->init(mSession);

	mRemoteCom->start();
}


void MultiplayerManager::disconnect()
{
	LOG_INFO("Starting Disconnect Sequence..");

	// First, send disconnect notification to remote (if connected)
	if (mSession && mSession->isConnected())
	{
		// Send disconnect state to remote before closing anything
		ConnectionStatusEvent disconnectEvent{ConnectionState::Disconnected};
		connectionStatusChanged(disconnectEvent);

		// Give a brief moment for the message to be sent
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
	}

	closeRemoteCommunication();

	closeDiscovery();
	closeTCPServerOrClient();

	{
		std::lock_guard<std::mutex> lock(mSessionMutex);
		if (mSession)
		{
			mSession->stopReadAsync();
			mSession.reset();
		}
	}

	LOG_INFO("Network Connection closed.");
}


void MultiplayerManager::onRemoteConnectionStateReceived(const ConnectionState &state)
{
	// We received a connection status change event from the remote
	mRemoteConnectionState = state;

	if (mConnectionState == ConnectionState::ConnectionRequested && state == ConnectionState::Connected)
		connectionStatusChanged(ConnectionState::SetPlayerColor);


	else if (state == ConnectionState::SetPlayerColor)
		connectionStatusChanged(ConnectionState::SetPlayerColor);


	else if (state == ConnectionState::GameStarted)
		connectionStatusChanged(ConnectionState::GameStarted);


	else if (state == ConnectionState::Disconnected)
		connectionStatusChanged(ConnectionState::Disconnected);
}


void MultiplayerManager::onRemoteInvitationReceived(const InvitationRequest &invite)
{
	Endpoint ep;
	ep.playerName = invite.playerName;

	ConnectionStatusEvent event;
	event.remoteEndpoint = ep;
	event.state			 = ConnectionState::ConnectionRequested;

	// TODO: Pre-Filter for version missmatch

	connectionStatusChanged(event);
}


void MultiplayerManager::onRemoteInvitationResponseReceived(const InvitationResponse &response)
{
	LOG_INFO("Invitation was {}", response.accepted ? "Accepted" : "Declined");

	ConnectionStatusEvent event;

	if (response.accepted)
	{
		event.state = ConnectionState::Connected;
	}
	else
	{
		event.state = ConnectionState::Disconnected; // For now just change to disconnected
	}

	connectionStatusChanged(event);
}


void MultiplayerManager::onRemotePlayerChosenReceived(const PlayerColor player)
{
	LOG_INFO("Received a player chosen message from the remote. Player = {}", LoggingHelper::playerColourToString(player));

	// The remote chose that player, so we switch locally to the corresponding oppsosite playercolor
	PlayerColor localPlayer = player == PlayerColor::White ? PlayerColor::Black : PlayerColor::White;

	LOG_INFO("So we set ourselves to Player {}", LoggingHelper::playerColourToString(localPlayer));

	remotePlayerChosen(localPlayer);
}


void MultiplayerManager::onRemotePlayerReadyFlagReceived(const bool flag)
{
	LOG_INFO("Received a remote ready for game flag: {}", LoggingHelper::boolToString(flag));
	setRemotePlayerReadyForGameFlag(flag);
}


void MultiplayerManager::setRemotePlayerReadyForGameFlag(const bool flag)
{
	mRemotePlayerReadyForGameFlag.store(flag);
	checkIfReadyForGame();
}


bool MultiplayerManager::checkIfReadyForGame()
{
	if (mLocalPlayerReadyForGameFlag.load() && mRemotePlayerReadyForGameFlag.load())
	{
		// Both are ready for game
		ConnectionStatusEvent event;
		event.state = ConnectionState::GameStarted;

		connectionStatusChanged(event);
		return true;
	}

	LOG_INFO("Local Ready = {}, Remote Ready = {}", mLocalPlayerReadyForGameFlag.load(), mRemotePlayerReadyForGameFlag.load());
	return false;
}


void MultiplayerManager::closeDiscovery()
{
	if (mDiscovery)
	{
		mDiscovery->deinit();
		mDiscovery.reset();
	}
}


void MultiplayerManager::closeTCPServerOrClient()
{
	if (mClient)
	{
		mClient.reset();
	}
	if (mServer)
	{
		mServer.reset();
	}
}


void MultiplayerManager::closeRemoteCommunication()
{
	if (mRemoteCom)
	{
		mRemoteCom->deinit();
		mRemoteCom->stop();
		mRemoteCom.reset();
	}
}


void MultiplayerManager::onNetworkAdapterChanged(const NetworkAdapter &adapter)
{
	mLocalIPv4 = adapter.IPv4;
}


bool MultiplayerManager::startDiscovery(const std::string IPv4, const int port, DiscoveryMode mode)
{
	if (mDiscovery)
		mDiscovery->deinit();

	mDiscovery.reset(new DiscoveryService(mIoContext));

	bool bindingSucceeded = mDiscovery->init(getLocalPlayerName(), IPv4, port);

	if (!bindingSucceeded)
	{
		ConnectionStatusEvent state = {ConnectionState::Error, "Failed to bind the discovery socket!"};
		connectionStatusChanged(state);
		return false;
	}

	mDiscovery->attachObserver(shared_from_this());

	mDiscovery->startDiscovery(mode);

	connectionStatusChanged(mode == DiscoveryMode::Server ? ConnectionState::HostingSession : ConnectionState::WaitingForARemote);

	return true;
}


void MultiplayerManager::setInternalObservers()
{
	if (!mRemoteCom || !mRemoteReceiver || !mRemoteSender)
	{
		LOG_ERROR("Could not set the observers, since the modules are not set up yet!");
		return;
	}

	LOG_INFO("Internal multiplayer observers set");

	// Remote Communication and Sender/Receiver Connection
	mRemoteCom->attachObserver(mRemoteReceiver);
	mRemoteSender->attachObserver(mRemoteCom);

	mRemoteReceiver->attachObserver(shared_from_this());

	this->attachObserver(mRemoteSender);
}


void MultiplayerManager::sendConnectRequest()
{
	std::string		  localVersion = "1.0.0"; // TODO: For now leave it at 1.0.0 -> create project.h.in file for CMake

	InvitationRequest invite;
	invite.playerName = getLocalPlayerName();
	invite.version	  = localVersion;

	mRemoteSender->sendConnectionInvite(invite);
}


void MultiplayerManager::sendConnectResponse(bool accepted, std::string reason)
{
	InvitationResponse response;
	response.accepted	= accepted;
	response.playerName = getLocalPlayerName();
	response.reason		= reason;

	mRemoteSender->sendConnectionResponse(response);
}


void MultiplayerManager::connectionStatusChanged(const ConnectionStatusEvent event)
{
	if (mConnectionState == event)
		return;

	// Update the connection state
	mConnectionState = event;

	// Notify all observers about the connection state change
	for (auto &observer : mObservers)
	{
		if (auto obs = observer.lock())
			obs->onConnectionStateChanged(event);
	}
}


void MultiplayerManager::localPlayerChosen(const PlayerColor localPlayer)
{
	if (mLocalPlayerColor == localPlayer)
		return;

	mLocalPlayerColor = localPlayer;

	for (auto &observer : mObservers)
	{
		if (auto obs = observer.lock())
			obs->onLocalPlayerChosen(localPlayer);
	}
}


void MultiplayerManager::remotePlayerChosen(const PlayerColor local)
{
	for (auto &observer : mObservers)
	{
		if (auto obs = observer.lock())
			obs->onRemotePlayerChosen(local);
	}
}


void MultiplayerManager::localReadyFlagSet(const bool flag)
{
	// Set the local ready flag
	mLocalPlayerReadyForGameFlag.store(flag);

	// Send local player ready to remote
	for (auto &observer : mObservers)
	{
		if (auto obs = observer.lock())
			obs->onLocalReadyFlagSet(flag);
	}

	// And check if both are ready
	checkIfReadyForGame();
}


void MultiplayerManager::onRemoteFound(const Endpoint &remote)
{
	mRemoteEndpoint = remote;
	connectionStatusChanged({ConnectionState::ClientFoundHost, remote});
}
