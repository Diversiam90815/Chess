/*
  ==============================================================================
	Module:         TCPClient
	Description:    Client implementation used for the multiplayer mode
  ==============================================================================
*/

#include "TCPClient.h"


TCPClient::TCPClient(boost::asio::io_context &ioContext) : mIoContext(ioContext) {}


void TCPClient::connect(const std::string &host, unsigned short port)
{
	// Create a new session
	auto		  session = TCPSession::create(mIoContext); // TCPSession constructor binds the socket to a port

	// Resolve host and port
	tcp::resolver resolver(mIoContext);
	auto		  endpoints = resolver.resolve(host, std::to_string(port));

	boost::asio::async_connect(session->socket(), endpoints,
							   [session, this](const boost::system::error_code &error, const tcp::endpoint &endpoint)
							   {
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
								   }
							   });
}


void TCPClient::setConnectHandler(ConnectHandler handler)
{
	mConnectHandler = handler;
}
