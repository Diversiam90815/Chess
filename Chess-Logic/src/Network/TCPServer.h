/*
  ==============================================================================
	Module:         TCPServer
	Description:    Server implementation used for the multiplayer mode
  ==============================================================================
*/


#pragma once

#include <boost/asio.hpp>
#include <functional>

#include "TCPSession.h"
#include "Logging.h"


using boost::asio::ip::tcp;

using SessionHandler = std::function<void(boost::shared_ptr<TCPSession> session)>; // Callback invoked when a new session is accepted


class TCPServer
{
	TCPServer(boost::asio::io_context &ioContext, unsigned short port);
	~TCPServer();

	void startAccept();

	void setSessionHandler(SessionHandler handler);


private:
	void					 handleAccept(boost::shared_ptr<TCPSession> session, const boost::system::error_code &error);


	boost::asio::io_context &mIoContext;

	tcp::acceptor			 mAcceptor;

	SessionHandler			 mSessionHandler;
};
