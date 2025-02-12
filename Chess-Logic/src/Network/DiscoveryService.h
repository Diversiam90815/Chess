/*
  ==============================================================================
	Class:          NetworkDiscovery
	Description:    Discovery of other Chess Apps on the local network using multicast packages
  ==============================================================================
*/

#pragma once

#include <boost/asio.hpp>
#include <boost/asio/steady_timer.hpp>
#include <boost/asio/ip/udp.hpp>

#include <functional>
#include <string>
#include <json.hpp>

#include "Logging.h"


using json	   = nlohmann::json;
namespace asio = boost::asio;
using boost::asio::ip::udp;


using PeerCallback = std::function<void(const json &)>;


struct Endpoint
{
	std::string IPAddress{};
	int			tcpPort{0};
	std::string playerName{};
};


// Enable automatic JSON conversion.
inline void to_json(json &j, const Endpoint &ep)
{
	j = json{{"IPAddress", ep.IPAddress}, {"tcpPort", ep.tcpPort}, {"playerName", ep.playerName}};
}

inline void from_json(const json &j, Endpoint &ep)
{
	j.at("IPAddress").get_to(ep.IPAddress);
	j.at("tcpPort").get_to(ep.tcpPort);
	j.at("playerName").get_to(ep.playerName);
}


class DiscoveryService
{
public:
	DiscoveryService(asio::io_context &ioContext);
	~DiscoveryService();

	bool init(std::string localIPv4, unsigned short tcpPort, const std::string &playerName);

	void start();

	void stop();

	void setPeerCallback(PeerCallback callback);

private:
	void				   sendPackage(std::string IPAddress = "255.255.255.255");

	void				   handleSend(const boost::system::error_code &error, size_t bytesSent);

	void				   startReceive();

	void				   handleReceive(const boost::system::error_code &error, size_t bytesReceived);


	const int			   mDiscoveryPort = 5555; // needs to be set from config later

	std::string			   localIPv4{};
	int					   mTcpPort{0};

	std::string			   mPlayerName{};



	std::vector<Endpoint>  mRemoteDevices;

	PeerCallback		   mPeerCallback;

	std::atomic<bool>	   mInitialized{false};
	std::atomic<bool>	   mIsRunning{false};


	asio::io_context	  *mIoContext = nullptr;

	udp::socket			   mSocket;

	udp::endpoint		   mSenderEndpoint;

	std::array<char, 1024> mRecvBuffer{};
};
