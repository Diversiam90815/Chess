/*
  ==============================================================================
	Module:         TCPSession
	Description:    Managing the socket and session used for the multiplayer mode
  ==============================================================================
*/

#include "TCPSession.h"


TCPSession::TCPSession(boost::asio::io_context &ioContext) : mSocket(ioContext)
{
	mSocket.open(tcp::v4());					  // Open the socket
	mSocket.bind(tcp::endpoint(tcp::v4(), 0));	  // Bind to a OS assigned port
	mBoundPort = mSocket.local_endpoint().port(); // Get the port number it is bound to

	mBuffer	   = new uint8_t[PackageBufferSize];
}


TCPSession::~TCPSession()
{
	free(mBuffer);
}


tcp::socket &TCPSession::socket()
{
	return mSocket;
}


bool TCPSession::sendMessage(MultiplayerMessageStruct &message)
{
	// Calculate sizes for the message's parts
	const size_t messageTypeSize	   = sizeof(message.type);
	const size_t messageDataSize	   = message.data.size();
	const size_t messageDataSizeLength = sizeof(messageDataSize);

	size_t		 offset				   = 0;

	memset(mBuffer, 0, PackageBufferSize); // Clear the buffer

	// Copy the secret identifier
	memcpy(&mBuffer[offset], RemoteComSecret, sizeof(RemoteComSecret));
	offset += sizeof(RemoteComSecret);

	// Copy the message type
	memcpy(&mBuffer[offset], &message.type, messageTypeSize);
	offset += messageTypeSize;

	// Copy the message data size
	memcpy(&mBuffer[offset], &messageDataSize, messageDataSizeLength);
	offset += messageDataSizeLength;

	// Copy the message data
	memcpy(&mBuffer[offset], message.data.data(), messageDataSize);
	offset += messageDataSize;

	try
	{
		boost::system::error_code ec;
		boost::asio::write(mSocket, boost::asio::buffer(mBuffer, offset), ec);

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


bool TCPSession::readMessage(MultiplayerMessageStruct &message)
{
	try
	{
		boost::system::error_code ec;
		size_t					  bytesRead = boost::asio::read(mSocket, boost::asio::buffer(mBuffer, PackageBufferSize), ec);

		if (ec)
		{
			LOG_ERROR("Error reading message: {}", ec.message());
			return false;
		}

		// Validate the secret identifier
		if (memcmp(mBuffer, RemoteComSecret, sizeof(RemoteComSecret)) != 0)
		{
			LOG_ERROR("Invalid message format: Secret identifier mismatch");
			return false;
		}

		size_t		 offset			 = sizeof(RemoteComSecret);

		// Extract message type
		const size_t messageTypeSize = sizeof(message.type);
		memcpy(&message.type, &mBuffer[offset], messageTypeSize);
		offset += messageTypeSize;

		// Extract message data size
		size_t		 dataLength			   = 0;
		const size_t messageDataSizeLength = sizeof(message.data.size());
		memcpy(&dataLength, &mBuffer[offset], messageDataSizeLength);
		offset += messageDataSizeLength;

		// Extract message data
		if ((offset + dataLength) > bytesRead)
			dataLength = bytesRead - offset; // We make sure we do not overflow.

		// Read the message data
		message.data = std::vector<uint8_t>(&mBuffer[offset], &mBuffer[offset + dataLength]);

		return true;
	}
	catch (const std::exception &e)
	{
		LOG_ERROR("Exception while reading message: {}", e.what());
		return false;
	}
}
