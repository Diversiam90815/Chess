/*
  ==============================================================================
	Module:         NetworkManager
	Description:    Managing the network part of this app
  ==============================================================================
*/

#include "NetworkManager.h"


NetworkManager::NetworkManager() {}


NetworkManager::~NetworkManager() {}


void NetworkManager::init()
{
	mNetworkInfo.init();

	mNetworkInfo.processAdapter();

	if (!setNetworkAdapterFromConfig()) // Will return false if we find an adapter in config file
	{
		presetNetworkAdapter();
	}

	setInitialized(true);
}


bool NetworkManager::hostSession()
{
	mServer = std::make_unique<TCPServer>(mIoContext);
	mServer->setSessionHandler([this](TCPSession::pointer session) { setTCPSession(session); });
	mServer->startAccept();
	mIoContext.run();

	if (isInitialized())
		return false;

	const std::string localIPv4 = mNetworkInfo.getCurrentNetworkAdapter().IPv4;
	const int		  port		= mServer->getBoundPort();
	bool			  success	= startServerDiscovery(localIPv4, port);
	return success;
}


void NetworkManager::joinSession(const Endpoint remote)
{
	mClient = std::make_unique<TCPClient>(mIoContext);
	mClient->setConnectHandler([this](TCPSession::pointer session) { setTCPSession(session); });
	mClient->connect(remote.IPAddress, remote.tcpPort);
	mIoContext.run();
}


void NetworkManager::setTCPSession(TCPSession::pointer session)
{
	mSession = session;
}


std::vector<NetworkAdapter> NetworkManager::getAvailableNetworkAdapters() const
{
	return mNetworkInfo.getAvailableNetworkAdapters();
}


bool NetworkManager::changeNetworkAdapter(const int ID)
{
	return mNetworkInfo.changeCurrentAdapter(ID);
}


int NetworkManager::getCurrentNetworkAdapterID()
{
	return mNetworkInfo.getCurrentNetworkAdapter().ID;
}


bool NetworkManager::presetNetworkAdapter()
{
	auto adapterInFile = FileManager::GetInstance()->readSelectedNetworkAdapter();

	if (adapterInFile.has_value())
		return false; // No need to preset if we have stored a selected adapter

	LOG_INFO("Since no adapter could been found in the config file, we are selecting one!");

	auto adapter = mNetworkInfo.getFirstEligibleAdapter();

	mNetworkInfo.setCurrentNetworkAdapter(adapter);
	FileManager::GetInstance()->setSelectedNetworkAdapter(adapter);

	return true;
}


bool NetworkManager::setNetworkAdapterFromConfig()
{
	auto adapterInFile = FileManager::GetInstance()->readSelectedNetworkAdapter();

	if (!adapterInFile.has_value())
		return false;

	LOG_INFO("Found a network adapter in the config file!");

	auto &userSetAdapter = adapterInFile.value();

	if (!mNetworkInfo.isAdapterCurrentlyAvailable(userSetAdapter))
		return false;

	mNetworkInfo.setCurrentNetworkAdapter(userSetAdapter);

	return true;
}


bool NetworkManager::startServerDiscovery(const std::string IPv4, const int port)
{
	mDiscovery.reset(new DiscoveryService(mIoContext));

	bool bindingSucceeded = mDiscovery->init(IPv4, port, getLocalPlayerName());
	mDiscovery->startSender();

	mIoContext.run();

	return bindingSucceeded;
}


void NetworkManager::startClientDiscovery()
{
	mDiscovery.reset(new DiscoveryService(mIoContext));

	mDiscovery->setPeerCallback([this](Endpoint remote) { joinSession(remote); });
	mDiscovery->startReceiver();

	mIoContext.run();
}
