/*
  ==============================================================================
	Module:         TCPClient
	Description:    Client implementation used for the multiplayer mode
  ==============================================================================
*/

#include "TCPClient.h"


TCPClient::TCPClient(asio::io_context &ioContext) : mIoContext(ioContext) {}


void TCPClient::connect(const std::string &host, unsigned short port)
{
	// Create a new session
	auto session = TCPSession::create(mIoContext); // TCPSession constructor binds the socket to a port

	// Create timer for the connection timeout
	auto timer	 = std::make_shared<asio::steady_timer>(mIoContext);
	timer->expires_after(std::chrono::seconds(mTimeoutInSeconds));

	// Start timeout timer
	timer->async_wait(
		[this, timer](const asio::error_code &ec)
		{
			if (!ec)
			{
				// Timer expired -> connection timed out
				if (mConnectTimeoutHandler)
				{
					mConnectTimeoutHandler();
				}
			}
		});

	// Resolve host and port
	tcp::resolver resolver(mIoContext);
	auto		  endpoints = resolver.resolve(host, std::to_string(port));

	asio::async_connect(session->socket(), endpoints,
							   [session, timer, this](const asio::error_code &error, const tcp::endpoint &endpoint)
							   {
								   // Cancel timeout timer
								   timer->cancel();

								   if (!error)
								   {
									   LOG_INFO("TCPClient connected to {}", endpoint.address().to_string().c_str());

									   if (mConnectHandler)
									   {
										   mConnectHandler(session);
									   }
								   }
								   else
								   {
									   LOG_ERROR("TCPClient connect error : {}!", error.message().c_str());

									   // Call the timeout handler if it is a timeout-related error
									   if (error == asio::error::timed_out || error == asio::error::connection_refused)
									   {
										   if (mConnectTimeoutHandler)
											   mConnectTimeoutHandler();
									   }
								   }
							   });
}


void TCPClient::setConnectHandler(ConnectHandler handler)
{
	mConnectHandler = handler;
}


void TCPClient::setConnectTimeoutHandler(ConnectTimeoutHandler handler)
{
	mConnectTimeoutHandler = handler;
}
