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
#include "UserSettings.h"


using asio::ip::udp;

/// <summary>
/// Enumerates the possible discovery modes.
/// </summary>
enum class DiscoveryMode
{
	None   = 1,
	Server = 2,
	Client = 3
};


/**
 * @brief	Provides network discovery services to detect remote devices advertising
 *			a multiplayer host or to broadcast this instance as a host.
 *
 * Thread model: Inherits ThreadBase. Network operations use async handlers
 * posted into supplied io_context.
 */
class DiscoveryService : public ThreadBase, public IDiscoveryObservable
{
public:
	DiscoveryService(asio::io_context &ioContext);
	~DiscoveryService();

	/**
	 * @brief	Initialize discovery context and prepare socket.
	 * @param	playerName -> Local display name.
	 * @param	localIPv4 -> IPv4 bound for outbound/broadcast.
	 * @param	tcpPort -> Intended TCP port for session negotiation.
	 * @return	true if initialization succeeded.
	 */
	bool	 init(const std::string &playerName, std::string localIPv4, unsigned short tcpPort);

	/**
	 * @brief	Deinitialize and close socket (safe to call multiple times).
	 */
	void	 deinit();

	/**
	 * @brief	Start discovery loop in provided mode (Host advertise / Client search).
	 */
	void	 startDiscovery(DiscoveryMode mode);

	/**
	 * @brief	Construct endpoint wrapper from IPv4 string (lookup inside known list if present).
	 */
	Endpoint getEndpointFromIP(const std::string &IPv4);

	/**
	 * @brief	Add a newly discovered remote endpoint (filtered for duplicates).
	 */
	void	 addRemoteToList(Endpoint remote);



private:
	/**
	 * @brief	Thread loop: schedules periodic broadcast & receive operations.
	 */
	void				   run() override;

	/**
	 * @brief	Send a discovery packet (advertise or probe).
	 */
	void				   sendPackage();

	/**
	 * @brief	Post async receive for incoming discovery packets.
	 */
	void				   receivePackage();

	/**
	 * @brief	Handle completion of async receive; parses and may register remote.
	 */
	void				   handleReceive(const asio::error_code &error, size_t bytesReceived);

	bool				   isInitialized() const { return mInitialized.load(); }

	/**
	 * @brief	Notify observers a remote was found (override from observable base).
	 */
	void				   remoteFound(const Endpoint &remote) override;


	int					   mDiscoveryPort{0};

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

	UserSettings		   mUserSettings;
};
