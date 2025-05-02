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
	if (tcpPort != 0 && !localIPv4.empty())
	{
		mTcpPort   = tcpPort;
		mLocalIPv4 = localIPv4;
	}

	mPlayerName = playerName;

	mSocket.open(udp::v4());

	// setting the local endpoint
	mLocalEndpoint = udp::endpoint(udp::v4(), mDiscoveryPort);
	boost::system::error_code	ec;

	// Setting the target endpoint
	boost::asio::ip::address_v4 address = boost::asio::ip::make_address_v4(broadCastAddress);
	mTargetEndpoint						= udp::endpoint(address, mDiscoveryPort);

	// Binding the socket to the local endpoint
	mSocket.bind(mLocalEndpoint, ec);
	if (ec)
	{
		LOG_ERROR("Failed to bind UDP Socket : {}", ec.message().c_str());
		return false;
	}

	// Turn broadcast on
	mSocket.set_option(boost::asio::socket_base::broadcast(true));

	mInitialized.store(true);
	return true;
}


void DiscoveryService::deinit()
{
	boost::system::error_code ec;
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

	run();
}


void DiscoveryService::run()
{
	// Start the first async receive operation
	receivePackage();

	while (isRunning())
	{
		// If in Server mode, periodically send discovery packages
		if (mDiscoveryMode == DiscoveryMode::Server)
		{
			sendPackage();
		}

		// Run IO context for processing async operations
		mIoContext->run_for(std::chrono::milliseconds(500));

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
	local.IPAddress					  = this->mLocalIPv4;
	local.playerName				  = this->mPlayerName;
	local.tcpPort					  = this->mTcpPort;

	json					  j		  = local;
	std::string				  message = j.dump();

	boost::system::error_code ec;

	// Sending the message
	size_t					  bytesSent = mSocket.send_to(boost::asio::buffer(message), mTargetEndpoint, 0, ec);

	if (ec)
	{
		LOG_ERROR("Error sending discovery package: {}", ec.message().c_str());
	}
	else
	{
		LOG_INFO("Discovery package sent ({} bytes)!", bytesSent);
	}
}


void DiscoveryService::receivePackage()
{
	mSocket.async_receive(asio::buffer(mRecvBuffer), [this](const boost::system::error_code &error, size_t bytesReceived) { handleReceive(error, bytesReceived); });
}


void DiscoveryService::handleReceive(const boost::system::error_code &error, size_t bytesReceived)
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

	// Continue receiving packages
	receivePackage();
}


void DiscoveryService::addRemoteToList(Endpoint remote)
{
	for (auto &ep : mRemoteDevices) // Detecting duplicates
	{
		if (ep == remote)
			return;
	}
	mRemoteDevices.push_back(remote);
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
