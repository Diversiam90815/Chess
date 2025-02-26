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
	stop();
}


bool DiscoveryService::init(std::string localIPv4, unsigned short tcpPort, const std::string &playerName)
{
	mTcpPort	= tcpPort;
	mPlayerName = playerName;

	mSocket.open(udp::v4());

	udp::endpoint			  localEndpoint(udp::v4(), mDiscoveryPort);
	boost::system::error_code ec;
	mSocket.bind(localEndpoint, ec);
	if (ec)
	{
		LOG_ERROR("Failed to bind UDP Socket : {}", ec.message().c_str());
		return false;
	}

	mSocket.set_option(boost::asio::socket_base::broadcast(true));

	mInitialized.store(true);
}


void DiscoveryService::startSender()
{
	if (!mInitialized.load())
	{
		LOG_ERROR("Discovery Service not initialized! Please initialize before calling to start!");
		return;
	}

	mIsRunning.store(true);

	scheduleNextSend();
}


void DiscoveryService::stop()
{
	mIsRunning.store(false);

	boost::system::error_code ec;
	mSocket.close(ec);
	if (ec)
	{
		LOG_ERROR("Error occurred during closing of the socket! Error : {}", ec.message().c_str());
	}

	mTimer.cancel();
}


void DiscoveryService::startReceiver()
{
	mSocket.async_receive(boost::asio::buffer(mRecvBuffer), std::bind(&DiscoveryService::handleReceive, this, std::placeholders::_1, std::placeholders::_2));
}


void DiscoveryService::setPeerCallback(PeerCallback callback)
{
	mPeerCallback = callback;
}


void DiscoveryService::sendPackage()
{
	if (!mIsRunning.load())
		return;

	boost::asio::ip::address_v4 address = boost::asio::ip::make_address_v4(broadCastAddress);

	udp::endpoint				target(address, mDiscoveryPort);

	Endpoint					local{};
	local.IPAddress		= this->localIPv4;
	local.playerName	= this->mPlayerName;
	local.tcpPort		= this->mTcpPort;

	json		j		= local;
	std::string message = j.dump();

	// Sending the message asynch
	mSocket.async_send_to(boost::asio::buffer(message), target, std::bind(&DiscoveryService::handleSend, this, std::placeholders::_1, std::placeholders::_2));
}


void DiscoveryService::scheduleNextSend()
{
	mTimer.expires_at(mTimer.expiry() + boost::asio::chrono::seconds(1));
	mTimer.async_wait(std::bind(&DiscoveryService::onSendTimer, this, std::placeholders::_1));
}


void DiscoveryService::onSendTimer(const boost::system::error_code &ec)
{
	if (!ec && mIsRunning.load())
	{
		sendPackage();

		scheduleNextSend();
	}
}


void DiscoveryService::handleSend(const boost::system::error_code &error, size_t bytesSent)
{
	if (error)
	{
		LOG_ERROR("Error sending discovery package: {}", error.message().c_str());
	}
	else
	{
		LOG_INFO("Discovery package sent ({} bytes)!", std::to_string(bytesSent).c_str());
	}
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

	if (mIsRunning.load())
	{
		startReceiver();
	}
}


void DiscoveryService::addRemoteToList(Endpoint remote)
{
	for (auto &ep : mRemoteDevices) // Detecting duplicates
	{
		if (ep == remote)
			return;
	}
	mRemoteDevices.push_back(remote);

	if (mPeerCallback)
		mPeerCallback(remote);
}
