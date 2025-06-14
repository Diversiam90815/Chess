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

	mServer = std::make_unique<TCPServer>(mIoContext);

	mServer->setSessionHandler([this](TCPSession::pointer session) { setTCPSession(session); });

	//// TODO
	// mServer->setConnectionRequestHandler(
	//	[this](const std::string &remoteIPv4)
	//	{
	//		ConnectionStatusEvent event{};
	//		event.state						= ConnectionState::ConnectionRequested;
	//		event.remoteEndpoint.IPAddress	= remoteIPv4;
	//		event.remoteEndpoint.tcpPort	= 1;		  // Leave 1 for now
	//		event.remoteEndpoint.playerName = remoteIPv4; // Use IPv4 as remote player name for now (@TODO)

	//		connectionStatusChanged(event);				  // Notify observers that we have a connection request
	//	});

	mServer->startAccept();

	const int port	  = mServer->getBoundPort();
	bool	  success = startServerDiscovery(mLocalIPv4, port);

	return success;
}


void MultiplayerManager::joinSession()
{
	if (!mRemoteEndpoint.isValid())
		return;

	connectionStatusChanged(ConnectionState::PendingHostApproval);

	mClient = std::make_unique<TCPClient>(mIoContext);

	mClient->setConnectHandler([this](TCPSession::pointer session) { setTCPSession(session); });
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
	
	setInternalObservers();
	
	mRemoteCom->start();

	connectionStatusChanged(ConnectionState::Connected);
}


TCPSession::pointer MultiplayerManager::getActiveSession()
{
	std::lock_guard<std::mutex> lock(mSessionMutex);
	return mSession;
}


void MultiplayerManager::disconnect()
{
	connectionStatusChanged(ConnectionState::Disconnecting);

	if (mDiscovery)
	{
		mDiscovery->stop();
		mDiscovery.reset();
	}

	mSession.reset();
	mClient.reset();
	mServer.reset();

	connectionStatusChanged(ConnectionState::Disconnected);

	LOG_INFO("Network Connection closed.");
}


void MultiplayerManager::onNetworkAdapterChanged(const NetworkAdapter &adapter)
{
	mLocalIPv4 = adapter.IPv4;
}


bool MultiplayerManager::startServerDiscovery(const std::string IPv4, const int port)
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

	mDiscovery->startDiscovery(DiscoveryMode::Server);

	mDiscovery->attachObserver(shared_from_this());

	connectionStatusChanged(ConnectionState::HostingSession);

	return bindingSucceeded;
}


void MultiplayerManager::startClientDiscovery()
{
	if (mDiscovery)
		mDiscovery->deinit();

	mDiscovery.reset(new DiscoveryService(mIoContext));
	bool bindingSucceeded = mDiscovery->init(getLocalPlayerName());

	if (!bindingSucceeded)
	{
		ConnectionStatusEvent state = {ConnectionState::Error, "Failed to bind socket in client discovery!"};

		connectionStatusChanged(state);
		return;
	}

	mDiscovery->startDiscovery(DiscoveryMode::Client);

	mDiscovery->attachObserver(shared_from_this());

	connectionStatusChanged(ConnectionState::WaitingForARemote);
}


void MultiplayerManager::setInternalObservers()
{
	if (!mRemoteCom || !mRemoteReceiver || !mRemoteSender)
	{
		LOG_ERROR("Could not set the observers, since the modules are not set up yet!");
		return;
	}

	// Remote Communication and Sender/Receiver Connection
	mRemoteCom->attachObserver(mRemoteReceiver);
	mRemoteSender->attachObserver(mRemoteCom);

	mNetwork->attachObserver(shared_from_this());
}


void MultiplayerManager::approveConnectionRequest()
{
	if (!mServer)
		return;

	mServer->respondToConnectionRequest(true);
}


void MultiplayerManager::rejectConnectionRequest()
{
	if (!mServer)
		return;

	mServer->respondToConnectionRequest(false);
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
