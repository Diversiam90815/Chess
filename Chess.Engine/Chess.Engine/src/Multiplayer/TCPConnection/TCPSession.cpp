/*
  ==============================================================================
	Module:         TCPSession
	Description:    Managing the socket and session used for the multiplayer mode
  ==============================================================================
*/

#include "TCPSession.h"


TCPSession::TCPSession(asio::io_context &ioContext) : mSocket(ioContext)
{
	mSocket.open(tcp::v4());						  // Open the socket
	mSocket.bind(tcp::endpoint(tcp::v4(), 0));		  // Bind to a OS assigned port
	mBoundPort	   = mSocket.local_endpoint().port(); // Get the port number it is bound to

	mSendBuffer	   = new uint8_t[PackageBufferSize];
	mReceiveBuffer = new uint8_t[PackageBufferSize];
}


TCPSession::TCPSession(tcp::socket &&socket) : mSocket(std::move(socket))
{
	mSendBuffer	   = new uint8_t[PackageBufferSize];
	mReceiveBuffer = new uint8_t[PackageBufferSize];
}


TCPSession::~TCPSession()
{
	delete[] mSendBuffer;
	delete[] mReceiveBuffer;
}


tcp::socket &TCPSession::socket()
{
	return mSocket;
}


bool TCPSession::isConnected() const
{
	if (!mSocket.is_open())
		return false;

	try
	{
		return !mSocket.remote_endpoint().address().is_unspecified();
	}
	catch (const std::exception &)
	{
		// If we can't get the remote endpoint, the connection is not valid
		return false;
	}
}


void TCPSession::readMessageAsync()
{
	if (!mAsyncReadActive || !isConnected())
		return;

	// Read full package of fixed size
	asio::async_read(mSocket, asio::buffer(mReceiveBuffer, PackageBufferSize), asio::transfer_at_least(sizeof(MultiplayerMessageType) + sizeof(size_t)),
					 [this](const asio::error_code &ec, size_t bytes_transfered)
					 {
						 if (!mAsyncReadActive)
							 return;

						 if (ec)
						 {
							 if (asio::error::eof == ec || asio::error::connection_reset == ec)
							 {
								 LOG_WARNING("Remote Socket closed or lost connection. We turn off async read for now!");
								 mAsyncReadActive = false;
								 return;
							 }

							 LOG_ERROR("Error reading message: {}", ec.message().c_str());

							 // If Connection still active, try reading again
							 if (isConnected() && mAsyncReadActive)
								 readMessageAsync();

							 return;
						 }

						 MultiplayerMessageStruct message;

						 // Extract message type
						 memcpy(&message.type, mReceiveBuffer, sizeof(MultiplayerMessageType));

						 // Extract the message size
						 size_t dataSize = 0;
						 memcpy(&dataSize, mReceiveBuffer + sizeof(MultiplayerMessageType), sizeof(size_t));

						 // Validate and limit data size to prevent buffer overflow
						 if (dataSize > PackageBufferSize - sizeof(MultiplayerMessageType) - sizeof(size_t))
						 {
							 LOG_ERROR("Received data size ({} bytes) exceeds maximum allowed!", dataSize);
							 readMessageAsync(); // Continue reading next message
							 return;
						 }

						 // Copy the actual message data
						 if (dataSize > 0)
						 {
							 message.data.assign(mReceiveBuffer + sizeof(MultiplayerMessageType) + sizeof(size_t),
												 mReceiveBuffer + sizeof(MultiplayerMessageType) + sizeof(size_t) + dataSize);
						 }

						 // Deliver message
						 if (mMessageReceivedCallback)
							 mMessageReceivedCallback(message);

						 readMessageAsync();
					 });
}


bool TCPSession::sendMessage(MultiplayerMessageStruct &message)
{
	if (!isConnected())
	{
		LOG_ERROR("Socket is not connected. Cannot send message.");
		return false;
	}

	// Calculate sizes for the message's parts
	const size_t messageTypeSize	   = sizeof(message.type);
	const size_t messageDataSize	   = message.data.size();
	const size_t messageDataSizeLength = sizeof(messageDataSize);

	size_t		 offset				   = 0;

	memset(mSendBuffer, 0, PackageBufferSize); // Clear the buffer

	// Copy the message type
	memcpy(&mSendBuffer[offset], &message.type, messageTypeSize);
	offset += messageTypeSize;

	// Copy the message data size
	memcpy(&mSendBuffer[offset], &messageDataSize, messageDataSizeLength);
	offset += messageDataSizeLength;

	// Copy the message data (secret + data)
	memcpy(&mSendBuffer[offset], message.data.data(), messageDataSize);
	offset += messageDataSize;

	try
	{
		asio::error_code ec;
		asio::write(mSocket, asio::buffer(mSendBuffer, offset), ec);

		if (ec)
		{
			LOG_ERROR("Error writing message: {}", ec.message());
			return false;
		}
		return true;
	}
	catch (const std::exception &e)
	{
		LOG_ERROR("Exception while writing message: {}", e.what());
		return false;
	}
}


void TCPSession::startReadAsync(MessageReceivedCallback callback)
{
	if (!isConnected())
	{
		LOG_ERROR("Cannot start async read on disconnected socket!");
		return;
	}

	// Init async read
	mMessageReceivedCallback = callback;
	mAsyncReadActive		 = true;

	// Start the read chain
	readMessageAsync();
}


void TCPSession::stopReadAsync()
{
	mAsyncReadActive = false;
}
