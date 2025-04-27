/*
  ==============================================================================
	Module:         NetworkDiscovery
	Description:    Discovery of other Chess Apps on the local network using multicast packages
  ==============================================================================
*/

#pragma once

#include <boost/asio.hpp>
#include <boost/asio/steady_timer.hpp>
#include <boost/asio/ip/udp.hpp>

#include <functional>
#include <string>

#include "Logging.h"
#include "JsonConversion.h"
#include "Discovery/DiscoveryEndpoint.h"


namespace asio = boost::asio;
using boost::asio::ip::udp;


using PeerCallback = std::function<void(const Endpoint &remote)>;


class DiscoveryService
{
public:
	DiscoveryService(asio::io_context &ioContext);
	~DiscoveryService();

	bool init(std::string localIPv4, unsigned short tcpPort, const std::string &playerName);

	void startSender();

	void stop();

	void startReceiver();

	void setPeerCallback(PeerCallback callback);


private:
	void					   sendPackage();

	void					   scheduleNextSend();

	void					   onSendTimer(const boost::system::error_code &ec);

	void					   handleSend(const boost::system::error_code &error, size_t bytesSent);

	void					   handleReceive(const boost::system::error_code &error, size_t bytesReceived);

	void					   addRemoteToList(Endpoint remote);


	const int				   mDiscoveryPort = 5555; // TODO: needs to be set from config later

	std::string				   localIPv4{};
	int						   mTcpPort{0};

	std::string				   mPlayerName{};

	std::vector<Endpoint>	   mRemoteDevices;

	PeerCallback			   mPeerCallback;

	std::atomic<bool>		   mInitialized{false};
	std::atomic<bool>		   mIsRunning{false};

	asio::io_context		  *mIoContext = nullptr;

	udp::socket				   mSocket;

	udp::endpoint			   mSenderEndpoint;

	std::array<char, 1024>	   mRecvBuffer{};

	std::string				   broadCastAddress = "255.255.255.255";

	boost::asio::system_timer  mTimer;
};
