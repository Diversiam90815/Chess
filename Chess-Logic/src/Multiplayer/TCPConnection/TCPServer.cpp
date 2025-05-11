/*
  ==============================================================================
	Module:         TCPServer
	Description:    Server implementation used for the multiplayer mode
  ==============================================================================
*/

#include "TCPServer.h"


TCPServer::TCPServer(asio::io_context &ioContext) : mIoContext(ioContext), mAcceptor(ioContext, tcp::endpoint(tcp::v4(), 0)) {}


TCPServer::~TCPServer() {}


void TCPServer::startAccept()
{
	auto session = TCPSession::create(mIoContext);
	mBoundPort	 = session->getBoundPort();

	mAcceptor.async_accept(session->socket(), [this, session](const asio::error_code &error) { handleAccept(session, error); });
}


void TCPServer::handleAccept(std::shared_ptr<TCPSession> session, const asio::error_code &error)
{
	if (!error)
	{
		LOG_INFO("TCP accepted connection from {}", session->socket().remote_endpoint().address().to_string().c_str());

		// Store pending session
		mPendingSession				  = session;

		const std::string &remoteIPv4 = mPendingSession->socket().remote_endpoint().address().to_string();

		// Notify that we have a connection request
		if (mConnectionRequestHandler)
		{
			mConnectionRequestHandler(remoteIPv4);
		}
		startAccept(); // Start accepting new conection
	}
	else
	{
		LOG_ERROR("TCPServer accept error {}!", error.message().c_str());
	}
}


void TCPServer::setSessionHandler(SessionHandler handler)
{
	mSessionHandler = handler;
}


void TCPServer::setConnectionRequestHandler(ConnectionRequestHandler handler)
{
	mConnectionRequestHandler = handler;
}


void TCPServer::respondToConnectionRequest(bool accepted)
{
	if (!mPendingSession)
		return;

	if (accepted)
	{
		LOG_INFO("Accepting connection from: {}", mPendingSession->socket().remote_endpoint().address().to_string().c_str());

		if (mSessionHandler)
		{
			mSessionHandler(mPendingSession);
		}
	}

	else
	{
		LOG_INFO("Rejecting connection from {}", mPendingSession->socket().remote_endpoint().address().to_string().c_str());

		// Close the socket
		asio::error_code ec;
		mPendingSession->socket().close(ec);
	}

	// Clear the pending session
	mPendingSession.reset();
}


const int TCPServer::getBoundPort() const
{
	return mBoundPort;
}
