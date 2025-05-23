/*
  ==============================================================================
	Module:         NetworkDiscovery
	Description:    Discovery of other Chess Apps on the local network using multicast packages
  ==============================================================================
*/

#pragma once

#include <asio.hpp>
#include <asio/steady_timer.hpp>
#include <asio/ip/udp.hpp>

#include <string>

#include "Logging.h"
#include "JsonConversion.h"
#include "Discovery/DiscoveryEndpoint.h"
#include "ThreadBase.h"
#include "IObservable.h"


using asio::ip::udp;


enum class DiscoveryMode
{
	None   = 1,
	Server = 2,
	Client = 3
};


class DiscoveryService : public ThreadBase, public IDiscoveryObservable
{
public:
	DiscoveryService(asio::io_context &ioContext);
	~DiscoveryService();

	bool init(const std::string &playerName, std::string localIPv4 = "", unsigned short tcpPort = 0);

	void deinit();

	void startDiscovery(DiscoveryMode mode);

private:
	void				   run() override;

	void				   sendPackage();

	void				   receivePackage();

	void				   handleReceive(const asio::error_code &error, size_t bytesReceived);

	void				   addRemoteToList(Endpoint remote);

	bool				   isInitialized() const { return mInitialized.load(); }

	void				   remoteFound(const Endpoint &remote) override;
	//void				   remoteSelected(const std::string &remoteName) override {};
	//void				   remoteRemoved(const std::string &remoteName) override {};

	const int			   mDiscoveryPort = 5555; // TODO: needs to be set from config later

	std::string			   mLocalIPv4{};
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

	const std::string	   broadCastAddress = "255.255.255.255";

	asio::steady_timer	   mTimer;
};
