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
	mBoundPort	 = mAcceptor.local_endpoint().port();

	LOG_INFO("Bound Port = {}", mBoundPort);

	mAcceptor.async_accept(
		[this](const asio::error_code &error, tcp::socket socket)
		{
			if (!error)
			{
				LOG_INFO("TCP accepted connection from {}", socket.remote_endpoint().address().to_string().c_str());

				// Store pending session
				auto newSession = TCPSession::create(std::move(socket));
				mPendingSession = newSession;

				if (mSessionHandler)
					mSessionHandler(mPendingSession);

				// We start waiting for invitation message
			}
		});
}


void TCPServer::setSessionHandler(SessionHandler handler)
{
	mSessionHandler = handler;
}


void TCPServer::respondToConnectionRequest(bool accepted)
{
	if (!mPendingSession)
		return;

	if (accepted)
	{
		LOG_INFO("Accepting connection from: {}", mPendingSession->socket().remote_endpoint().address().to_string().c_str());

		if (mSessionHandler)
			mSessionHandler(mPendingSession);
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


int TCPServer::getBoundPort() const
{
	return mBoundPort;
}
