/*
  ==============================================================================
	Module:         TCPClient
	Description:    Client implementation used for the multiplayer mode
  ==============================================================================
*/

#pragma once

#include <boost/asio.hpp>
#include <functional>
#include <memory>

#include "TCPSession.h"
#include "Logging.h"


using boost::asio::ip::tcp;

using ConnectHandler = std::function<void(std::shared_ptr<TCPSession> session)>;


class TCPClient
{
public:
	TCPClient(boost::asio::io_context &ioContext);
	~TCPClient() = default;

	void connect(const std::string &host, unsigned short port, ConnectHandler handler);

private:
	boost::asio::io_context &mIoContext;
};
