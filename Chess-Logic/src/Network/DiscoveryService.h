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


using json	   = nlohmann::json;
namespace asio = boost::asio;
using boost::asio::ip::udp;

using PeerCallback = std::function<void(const json &)>;


struct RemoteEndpoint
{
	std::string IPAddress{};
	int			tcpPort{0};
	std::string playerName{};
};


// Enable automatic JSON conversion.
inline void to_json(json &j, const RemoteEndpoint &ep)
{
	j = json{{"IPAddress", ep.IPAddress}, {"tcpPort", ep.tcpPort}, {"playerName", ep.playerName}};
}

inline void from_json(const json &j, RemoteEndpoint &ep)
{
	j.at("IPAddress").get_to(ep.IPAddress);
	j.at("tcpPort").get_to(ep.tcpPort);
	j.at("playerName").get_to(ep.playerName);
}


class DiscoveryService
{
public:
	DiscoveryService();
	~DiscoveryService();

	void init(asio::io_context &io_context, unsigned short tcpPort, const std::string &playerName);

	void start();

	void stop();

	void setPeerCallback(PeerCallback callback);

private:
	void						sendPackage(std::string IPAddress = "255.255.255.255");

	void						handleSend(const boost::system::error_code &error, size_t bytesSent);

	void						startReceive();

	void						handleReceive(const boost::system::error_code &error, size_t bytesReceived);


	const int					mDiscoveryPort = 5555; // needs to be set from config later

	int							tcpPort{0};

	std::string					mPlayerName{};



	std::vector<RemoteEndpoint> mRemoteDevices;

	PeerCallback				mPeerCallback;

	std::atomic<bool>			mInitialized{false};
	std::atomic<bool>			mIsRunning{false};


	asio::io_context		   *mIoContext = nullptr;

	udp::socket					mSocket;

	udp::endpoint				mSenderEndpoint;

	std::array<char, 1024>		mRecvBuffer;
};
