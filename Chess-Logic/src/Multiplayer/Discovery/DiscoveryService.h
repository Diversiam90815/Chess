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

#include <string>

#include "Logging.h"
#include "JsonConversion.h"
#include "Discovery/DiscoveryEndpoint.h"
#include "ThreadBase.h"


namespace asio = boost::asio;
using boost::asio::ip::udp;


enum class DiscoveryMode
{
	None   = 1,
	Server = 2,
	Client = 3
};


class DiscoveryService : public ThreadBase
{
public:
	DiscoveryService(asio::io_context &ioContext);
	~DiscoveryService();

	bool init(std::string localIPv4, unsigned short tcpPort, const std::string &playerName);

	void deinit();

	void startDiscovery(DiscoveryMode mode);

private:
	void				   run() override;

	void				   sendPackage();

	void				   receivePackage();

	void				   handleReceive(const boost::system::error_code &error, size_t bytesReceived);

	void				   addRemoteToList(Endpoint remote);

	bool				   isInitialized() const { return mInitialized.load(); }


	const int			   mDiscoveryPort = 5555; // TODO: needs to be set from config later

	std::string			   localIPv4{};
	int					   mTcpPort{0};

	std::string			   mPlayerName{};

	std::vector<Endpoint>  mRemoteDevices;

	std::atomic<bool>	   mInitialized{false};

	asio::io_context	  *mIoContext = nullptr;

	udp::socket			   mSocket;

	udp::endpoint		   mLocalEndpoint;
	udp::endpoint		   mTargetEndpoint;

	std::array<char, 1024> mRecvBuffer{};

	DiscoveryMode		   mDiscoveryMode{DiscoveryMode::None};

	std::string			   broadCastAddress = "255.255.255.255";

	asio::steady_timer	   mTimer;
};
