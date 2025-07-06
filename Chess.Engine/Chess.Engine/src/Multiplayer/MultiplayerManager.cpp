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

	mNetwork		= std::make_unique<NetworkManager>();
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


void MultiplayerManager::init()
{
	mNetwork->init();
}


void MultiplayerManager::reset()
{
	// Stop discovery if running
	mDiscovery->stop();

	// TODO: Reset server / client
}


bool MultiplayerManager::hostSession()
{
	if (mLocalIPv4.empty())
		return false;

	LOG_INFO("Hosting a session..");

	mServer = std::make_unique<TCPServer>(mIoContext);

	mServer->setSessionHandler([this](TCPSession::pointer session) { setTCPSession(session); });
	mServer->setConnectionRequestHandler([this](const std::string &remoteIPv4) { receivedInviteFromClient(remoteIPv4); });

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
		[this](TCPSession::pointer session)
		{
			setTCPSession(session);
			sendConnectRequest();
		});
	mClient->setConnectTimeoutHandler([this]() { connectionStatusChanged({ConnectionState::Error, "Connection request timed out or was rejected!"}); });

	mClient->connect(mRemoteEndpoint.IPAddress, mRemoteEndpoint.tcpPort);
}


void MultiplayerManager::setTCPSession(TCPSession::pointer session)
{
	{
		std::lock_guard<std::mutex> lock(mSessionMutex);
		mSession = session;
	}

	mRemoteCom->init(mSession);

	mRemoteCom->start();

	//// As we now have a connection, we can close the Discovery and the TCPServer/TCPClient
	// closeDiscovery();
	// closeTCPServerOrClient();

	// connectionStatusChanged(ConnectionState::Connected);
}


TCPSession::pointer MultiplayerManager::getActiveSession()
{
	std::lock_guard<std::mutex> lock(mSessionMutex);
	return mSession;
}


void MultiplayerManager::disconnect()
{
	connectionStatusChanged(ConnectionState::Disconnecting);

	closeDiscovery();
	closeRemoteCommunication();
	closeTCPServerOrClient();

	mSession.reset();

	connectionStatusChanged(ConnectionState::Disconnected);

	LOG_INFO("Network Connection closed.");
}


void MultiplayerManager::onRemoteConnectionStateReceived(const ConnectionState &state)
{
	// We received a connection status change event from the remote
	mRemoteConnectionState = state;

	if (mConnectionState == ConnectionState::Connected && state == ConnectionState::Connected)
	{
		// We both are connected, so we switch into setCurrentPlayer state
		connectionStatusChanged(ConnectionState::SetPlayerColor);
	}

	else if (state == ConnectionState::SetPlayerColor)
	{
		connectionStatusChanged(ConnectionState::SetPlayerColor);
	}

	else if (state == ConnectionState::GameStarted)
	{
		connectionStatusChanged(ConnectionState::GameStarted);
	}
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
		mRemoteCom.reset();
	}
}


void MultiplayerManager::receivedInviteFromClient(const std::string remoteIPv4)
{
	// Get remote endpoint
	Endpoint remoteEndpoint = mDiscovery->getEndpointFromIP(remoteIPv4);

	// Check if the endpoint is valid
	if (!remoteEndpoint.isValid())
	{
		LOG_ERROR("Could not get a remote endpoint from Discovery that matches the IPv4 we received a connect request from! IPv4 : {}", remoteIPv4.c_str());
		return;
	}

	// Set the connection event and status
	ConnectionStatusEvent event{};
	event.state			 = ConnectionState::ConnectionRequested;
	event.remoteEndpoint = remoteEndpoint;

	connectionStatusChanged(event);
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

	mNetwork->attachObserver(shared_from_this());

	mRemoteReceiver->attachObserver(shared_from_this());

	this->attachObserver(mRemoteSender);
}


void MultiplayerManager::sendConnectRequest()
{
	std::string		  localVersion = "1.0.0"; // TODO: For now leave it at 1.0.0 -> create project.h.in file for CMake

	InvitationRequest invite;
	invite.playerName = mLocalPlayerName;
	invite.version	  = localVersion;

	mRemoteSender->sendConnectionInvite(invite);
}


void MultiplayerManager::sendConnectResponse(bool accepted, std::string reason)
{
	InvitationResponse response;
	response.accepted	= accepted;
	response.playerName = mLocalPlayerName;
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


void MultiplayerManager::onRemoteFound(const Endpoint &remote)
{
	mRemoteEndpoint = remote;
	connectionStatusChanged({ConnectionState::ClientFoundHost, remote});
}


std::vector<NetworkAdapter> MultiplayerManager::getNetworkAdapters()
{
	return mNetwork->getAvailableNetworkAdapters();
}


bool MultiplayerManager::changeCurrentNetworkAdapter(int ID)
{
	auto adapters = mNetwork->getAvailableNetworkAdapters();

	for (auto &adapter : adapters)
	{
		if (adapter.ID != ID)
			continue;

		mNetwork->networkAdapterChanged(adapter);
		return true;
	}
	return false;
}


int MultiplayerManager::getCurrentNetworkAdapterID()
{
	return mNetwork->getCurrentNetworkAdapterID();
}
