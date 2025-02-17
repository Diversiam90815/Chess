/*
  ==============================================================================
	Class:          TCPServer
	Description:    Server implementation used for the multiplayer mode
  ==============================================================================
*/

#include "TCPServer.h"


TCPServer::TCPServer(boost::asio::io_context &ioContext, unsigned short port) : mIoContext(ioContext), mAcceptor(ioContext, tcp::endpoint(tcp::v4(), port)) {}


TCPServer::~TCPServer() {}


void TCPServer::startAccept()
{
	auto session = std::make_shared<TCPSession>(mIoContext);
	mAcceptor.async_accept(session->socket(), [this, session](const boost::system::error_code &error) { handleAccept(session, error); });
}


void TCPServer::setSessionHandler(SessionHandler handler)
{
	mSessionHandler = handler;
}


void TCPServer::handleAccept(std::shared_ptr<TCPSession> session, const boost::system::error_code &error)
{
	if (!error)
	{
		LOG_INFO("TCP accepted connection from {}", session->socket().remote_endpoint().address().to_string().c_str());
		session->start(); // Start session's async operation

		if (mSessionHandler)
		{
			mSessionHandler(session);
		}
	}
	else
	{
		LOG_ERROR("TCPServer accept error {}!", error.message().c_str());
	}

	startAccept(); // Start accepting new conection
}
