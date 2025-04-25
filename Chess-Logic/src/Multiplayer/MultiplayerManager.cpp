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
	mWorkerThread = std::thread([this]() { mIoContext.run(); });
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
	mClient = std::make_unique<TCPClient>(mIoContext);
	mClient->setConnectHandler([this](TCPSession::pointer session) { setTCPSession(session); });
	mClient->connect(remote.IPAddress, remote.tcpPort);
}


void MultiplayerManager::setTCPSession(TCPSession::pointer session)
{
	mSession = session;
}


TCPSession::pointer MultiplayerManager::getActiveSession()
{
	return mSession;
}


void MultiplayerManager::disconnect()
{
	if (mDiscovery)
	{
		mDiscovery->stop();
		mDiscovery.reset();
	}

	mSession.reset();
	mClient.reset();
	mServer.reset();

	LOG_INFO("Network Connection closed.");
}



void MultiplayerManager::onNetworkAdapterChanged(const NetworkAdapter &adapter)
{
	mLocalIPv4 = adapter.IPv4;
}


bool MultiplayerManager::startServerDiscovery(const std::string IPv4, const int port)
{
	mDiscovery.reset(new DiscoveryService(mIoContext));

	bool bindingSucceeded = mDiscovery->init(IPv4, port, getLocalPlayerName());
	mDiscovery->startSender();

	return bindingSucceeded;
}


void MultiplayerManager::startClientDiscovery()
{
	mDiscovery.reset(new DiscoveryService(mIoContext));

	mDiscovery->setPeerCallback([this](Endpoint remote) { joinSession(remote); });
	mDiscovery->startReceiver();
}
