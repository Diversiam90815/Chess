/*
  ==============================================================================
	Module:         MultiplayerManager
	Description:    Managing the multiplayer game mode
  ==============================================================================
*/

#include "MultiplayerManager.h"



MultiplayerManager::MultiplayerManager() : mWorkGuard(boost::asio::make_work_guard(mIoContext))
{
	// Start the IO Context in a dedicated thread
	mWorkerThread	= std::thread([this]() { mIoContext.run(); });

	mRemoteCom		= std::make_shared<RemoteCommunication>();
	mRemoteReceiver = std::make_shared<RemoteReceiver>();
	mRemoteSender	= std::make_shared<RemoteSender>();
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


bool MultiplayerManager::hostSession()
{
	mServer = std::make_unique<TCPServer>(mIoContext);
	mServer->setSessionHandler([this](TCPSession::pointer session) { setTCPSession(session); });
	mServer->startAccept();

	if (mLocalIPv4.empty())
		return false;

	const int port	  = mServer->getBoundPort();
	bool	  success = startServerDiscovery(mLocalIPv4, port);

	return success;
}


void MultiplayerManager::joinSession(const Endpoint remote)
{
	connectionStatusChanged(ConnectionState::Connecting);

	mClient = std::make_unique<TCPClient>(mIoContext);
	mClient->setConnectHandler([this](TCPSession::pointer session) { setTCPSession(session); });
	mClient->connect(remote.IPAddress, remote.tcpPort);
}


void MultiplayerManager::setTCPSession(TCPSession::pointer session)
{
	{
		std::lock_guard<std::mutex> lock(mSessionMutex);
		mSession = session;
	}

	mRemoteCom->init(mSession);

	setInternalObservers();

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
	mDiscovery.reset(new DiscoveryService(mIoContext));

	bool bindingSucceeded = mDiscovery->init(getLocalPlayerName(), IPv4, port);

	if (!bindingSucceeded)
	{
		connectionStatusChanged(ConnectionState::Error, "Failed to bind the discovery socket!");
		return;
	}

	mDiscovery->startDiscovery(DiscoveryMode::Server);

	connectionStatusChanged(ConnectionState::HostingSession);

	return bindingSucceeded;
}


void MultiplayerManager::startClientDiscovery()
{
	mDiscovery.reset(new DiscoveryService(mIoContext));
	bool bindingSucceeded = mDiscovery->init(getLocalPlayerName());

	if (!bindingSucceeded)
	{
		connectionStatusChanged(ConnectionState::Error, "Failed to bind socket in client discovery!");
		return;
	}

	mDiscovery->startDiscovery(DiscoveryMode::Client);
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
}


void MultiplayerManager::connectionStatusChanged(ConnectionState state, const std::string &errorMessage)
{
	if (mConnectionState == state)
		return;

	// Update the connection state
	mConnectionState.store(state);

	// Notify all observers about the connection state change
	for (auto &observer : mObservers)
	{
		if (auto obs = observer.lock())
			obs->onConnectionStateChanged(state, errorMessage);
	}
}
