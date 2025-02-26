/*
  ==============================================================================
	Module:         TCPSession
	Description:    Managing the socket and session used for the multiplayer mode
  ==============================================================================
*/

#include "TCPSession.h"


TCPSession::TCPSession(boost::asio::io_context &ioContext) : mSocket(ioContext)
{
	mSocket.open(tcp::v4());						 // Open the socket
	mSocket.bind(tcp::endpoint(tcp::v4(), 0));		 // Bind to a OS assigned port
	int boundPort = mSocket.local_endpoint().port(); // Get the port number it is bound to
}


tcp::socket &TCPSession::socket()
{
	return mSocket;
}


void TCPSession::start()
{
	mSocket.async_read_some(boost::asio::buffer(mData, max_length)),
		boost::bind(&TCPSession::handleRead, this, boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred);
}


void TCPSession::handleRead(const boost::system::error_code &error, size_t bytesRead)
{
	if (!error)
	{
		boost::asio::async_write(mSocket, boost::asio::buffer(mData, bytesRead), boost::bind(&TCPSession::handleWrite, this, boost::asio::placeholders::error));
	}
	else
	{
		delete this;
	}
}


void TCPSession::handleWrite(const boost::system::error_code &error)
{
	if (!error)
	{
		mSocket.async_read_some(boost::asio::buffer(mData, max_length),
								boost::bind(&TCPSession::handleRead, this, boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));
	}
	else
	{
		delete this;
	}
}
