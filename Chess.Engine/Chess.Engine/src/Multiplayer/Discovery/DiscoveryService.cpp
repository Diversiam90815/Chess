/*
  ==============================================================================
	Module:         NetworkDiscovery
	Description:    Discovery of other Chess Apps on the local network using multicast packages
  ==============================================================================
*/

#include "DiscoveryService.h"


DiscoveryService::DiscoveryService(asio::io_context &ioContext) : mSocket(ioContext), mTimer(ioContext)
{
	mIoContext = &ioContext;
}


DiscoveryService::~DiscoveryService()
{
	deinit();
}


bool DiscoveryService::init(const std::string &playerName, std::string localIPv4, unsigned short tcpPort)
{
	if (localIPv4.empty() || playerName.empty())
		return false;

	mTcpPort	= tcpPort;
	mLocalIPv4	= localIPv4;
	mPlayerName = playerName;

	mSocket.open(udp::v4());

	// setting the local endpoint
	mLocalEndpoint = udp::endpoint(udp::v4(), mDiscoveryPort);
	asio::error_code	 ec;

	// Setting the target endpoint
	asio::ip::address_v4 address = asio::ip::make_address_v4(broadCastAddress);
	mTargetEndpoint				 = udp::endpoint(address, mDiscoveryPort);

	// Binding the socket to the local endpoint
	mSocket.bind(mLocalEndpoint, ec);
	if (ec)
	{
		LOG_ERROR("Failed to bind UDP Socket : {}", ec.message().c_str());
		return false;
	}

	// Turn broadcast on
	mSocket.set_option(asio::socket_base::broadcast(true));

	mInitialized.store(true);
	return true;
}


void DiscoveryService::deinit()
{
	asio::error_code ec;

	// Cancel any pending async operations on the socket
	mSocket.cancel(ec);
	if (ec)
	{
		LOG_ERROR("Error cancelling socket operations: {}", ec.message().c_str());
	}

	mSocket.close(ec);
	if (ec)
	{
		LOG_ERROR("Error occurred during closing of the socket! Error : {}", ec.message().c_str());
	}

	mTimer.cancel();

	// stop the thread
	stop();
}


void DiscoveryService::startDiscovery(DiscoveryMode mode)
{
	if (!isInitialized())
	{
		throw std::runtime_error("Discovery Service has not been initialized but was called to start!");
		return;
	}

	mDiscoveryMode = mode;

	if (mDiscoveryMode == DiscoveryMode::Server)
	{
		LOG_INFO("Starting discovery server...");
	}
	else if (mDiscoveryMode == DiscoveryMode::Client)
	{
		LOG_INFO("Starting discovery client...");
	}
	else
	{
		LOG_ERROR("Invalid discovery mode!");
		return;
	}

	start();
}


Endpoint DiscoveryService::getEndpointFromIP(const std::string &IPv4)
{
	for (auto &endpoint : mRemoteDevices)
	{
		if (endpoint.IPAddress == IPv4)
			return endpoint;
	}
	return {};
}


void DiscoveryService::run()
{
	while (isRunning())
	{
		// Start the first async receive operation
		receivePackage();

		sendPackage();

		// Sleep or wait for event
		waitForEvent(200);
	}
}


void DiscoveryService::sendPackage()
{
	if (!isInitialized())
	{
		LOG_ERROR("Tried to start the Discovery in Server mode without initializing first! Please initialize before attempting to start the Discovery in Server Mode!");
		return;
	}

	Endpoint local{};
	local.IPAddress			 = this->mLocalIPv4;
	local.playerName		 = this->mPlayerName;
	local.tcpPort			 = this->mTcpPort;

	json			 j		 = local;
	std::string		 message = j.dump();

	asio::error_code ec;

	// Sending the message
	size_t			 bytesSent = mSocket.send_to(asio::buffer(message), mTargetEndpoint, 0, ec);

	if (ec)
	{
		LOG_ERROR("Error sending discovery package: {}", ec.message().c_str());
	}
	else
	{
		LOG_DEBUG("Discovery package sent ({} bytes)!", bytesSent);
	}
}


void DiscoveryService::receivePackage()
{
	mSocket.async_receive(asio::buffer(mRecvBuffer), [this](const asio::error_code &error, size_t bytesReceived) { handleReceive(error, bytesReceived); });
}


void DiscoveryService::handleReceive(const asio::error_code &error, size_t bytesReceived)
{
	if (!error && bytesReceived > 0)
	{
		try
		{
			std::string receivedData(mRecvBuffer.data(), bytesReceived);
			json		j	   = json::parse(receivedData);

			Endpoint	remote = j.get<Endpoint>();

			addRemoteToList(remote);
		}
		catch (std::exception &e)
		{
			LOG_ERROR("Error parsing discovery package: {}", e.what());
		}
	}
	else if (error)
	{
		LOG_WARNING("Receive error occurred: {}", error.message().c_str());
	}
}


void DiscoveryService::addRemoteToList(Endpoint remote)
{
	for (auto &ep : mRemoteDevices) // Detecting duplicates
	{
		if (ep == remote)
			return;
	}

	if (mLocalIPv4 == remote.IPAddress) // Don't add this device
		return;

	LOG_INFO("Found remote: IPv4 : {0}, Port: {1}, Name: {2}", remote.IPAddress.c_str(), remote.tcpPort, remote.playerName.c_str());

	mRemoteDevices.push_back(remote);

	if (mDiscoveryMode == DiscoveryMode::Client) //	Notify Observers only in Client mode
		remoteFound(remote);
}


void DiscoveryService::remoteFound(const Endpoint &remote)
{
	for (auto &observer : mObservers)
	{
		if (auto obs = observer.lock())
			obs->onRemoteFound(remote);
	}
}
