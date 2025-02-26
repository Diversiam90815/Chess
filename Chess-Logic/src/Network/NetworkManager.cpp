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

	mDiscovery = std::make_unique<DiscoveryService>();
	//mDiscovery->init();

	setInitialized(true);
}


bool NetworkManager::hostSession()
{
	mServer = std::make_unique<TCPServer>(mNetworkInfo.getCurrentNetworkAdapter().ID);
	mServer->setSessionHandler([this](TCPSession::pointer session) { setTCPSession(session); });
	mServer->startAccept();

	if (isInitialized())
		return false;

	const std::string localIPv4 = mNetworkInfo.getCurrentNetworkAdapter().IPv4;
	const int		  port		= mServer->getBoundPort();
	startDiscovery(localIPv4, port);
}


void NetworkManager::joinSession(const std::string IPv4, const int port)
{
	mClient = std::make_unique<TCPClient>();
	mClient->setConnectHandler([this](TCPSession::pointer session) { setTCPSession(session); });
	mClient->connect(IPv4, port);
}


void NetworkManager::setTCPSession(TCPSession::pointer session)
{
	mSession = session;
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

	LOG_INFO("Set user defined adapter to :");
	LOG_INFO("\t Description:\t {}", adapter.description);
	LOG_INFO("\t IPv4: \t\t\t{}", adapter.IPv4);
	LOG_INFO("\t Subnet: \t\t{}", adapter.subnet);
	LOG_INFO("\t ID: \t\t\t{}", adapter.ID);

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

	LOG_INFO("Set user defined adapter to :");
	LOG_INFO("\t Description:\t {}", userSetAdapter.description);
	LOG_INFO("\t IPv4: \t\t\t{}", userSetAdapter.IPv4);
	LOG_INFO("\t Subnet: \t\t{}", userSetAdapter.subnet);
	LOG_INFO("\t ID: \t\t\t{}", userSetAdapter.ID);
	mNetworkInfo.setCurrentNetworkAdapter(userSetAdapter);

	return true;
}


void NetworkManager::startDiscovery(const std::string IPv4, const int port) {}
