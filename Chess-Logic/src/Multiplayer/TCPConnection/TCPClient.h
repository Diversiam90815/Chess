/*
  ==============================================================================
	Module:         TCPClient
	Description:    Client implementation used for the multiplayer mode
  ==============================================================================
*/

#pragma once

#include <asio.hpp>
#include <functional>
#include <memory>

#include "TCPSession.h"
#include "Logging.h"


using asio::ip::tcp;

using ConnectHandler		= std::function<void(std::shared_ptr<TCPSession> session)>;

using ConnectTimeoutHandler = std::function<void()>;


class TCPClient
{
public:
	TCPClient(asio::io_context &ioContext);
	~TCPClient() = default;

	void connect(const std::string &host, unsigned short port);

	void setConnectHandler(ConnectHandler handler);
	void setConnectTimeoutHandler(ConnectTimeoutHandler handler);

private:
	ConnectHandler		  mConnectHandler;
	ConnectTimeoutHandler mConnectTimeoutHandler;

	const int			  mTimeoutInSeconds = 10;

	asio::io_context	 &mIoContext;
};
