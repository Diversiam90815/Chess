/*
  ==============================================================================
	Module:         TCPClient
	Description:    Client implementation used for the multiplayer mode
  ==============================================================================
*/

#pragma once

#include <asio.hpp>
#include <memory>

#include "TCPInterfaces.h"
#include "TCPSession.h"
#include "Logging.h"


using asio::ip::tcp;


/**
 * @brief Implements a TCP client responsible for connecting to a remote host
 *        and establishing a TCPSession.
 */
class TCPClient : public ITCPClient
{
public:
	TCPClient(asio::io_context &ioContext);
	~TCPClient() = default;

	/**
	 * @brief Initiate asynchronous connection to host:port.
	 */
	void connect(const std::string &host, unsigned short port) override;

	void setConnectHandler(ConnectHandler handler) override;
	void setConnectTimeoutHandler(ConnectTimeoutHandler handler) override;

private:
	ConnectHandler		  mConnectHandler;
	ConnectTimeoutHandler mConnectTimeoutHandler;

	const int			  mTimeoutInSeconds = 10;

	asio::io_context	 &mIoContext;
};
