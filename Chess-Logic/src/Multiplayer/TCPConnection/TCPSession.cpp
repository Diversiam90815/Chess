/*
  ==============================================================================
	Module:         TCPSession
	Description:    Managing the socket and session used for the multiplayer mode
  ==============================================================================
*/

#include "TCPSession.h"


TCPSession::TCPSession(boost::asio::io_context &ioContext) : mSocket(ioContext)
{
	mSocket.open(tcp::v4());						  // Open the socket
	mSocket.bind(tcp::endpoint(tcp::v4(), 0));		  // Bind to a OS assigned port
	mBoundPort	   = mSocket.local_endpoint().port(); // Get the port number it is bound to

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
	return mSocket.is_open() && !mSocket.remote_endpoint().address().is_unspecified();
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

	// Copy the secret identifier
	memcpy(&mSendBuffer[offset], RemoteComSecret, sizeof(RemoteComSecret));
	offset += sizeof(RemoteComSecret);

	// Copy the message type
	memcpy(&mSendBuffer[offset], &message.type, messageTypeSize);
	offset += messageTypeSize;

	// Copy the message data size
	memcpy(&mSendBuffer[offset], &messageDataSize, messageDataSizeLength);
	offset += messageDataSizeLength;

	// Copy the message data
	memcpy(&mSendBuffer[offset], message.data.data(), messageDataSize);
	offset += messageDataSize;

	try
	{
		boost::system::error_code ec;
		boost::asio::write(mSocket, boost::asio::buffer(mSendBuffer, offset), ec);

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
	if (!isConnected())
	{
		LOG_ERROR("Socket is not connected. Cannot read message.");
		return false;
	}

	try
	{
		boost::system::error_code ec;

		// Calculate header size
		const size_t			  headerSize = sizeof(RemoteComSecret) + sizeof(message.type) + sizeof(size_t);

		// First phase: Read the header
		size_t					  bytesRead	 = boost::asio::read(mSocket, boost::asio::buffer(mReceiveBuffer, headerSize), boost::asio::transfer_exactly(headerSize), ec);

		// Check for error
		if (ec)
		{
			LOG_ERROR("Error reading message: {}", ec.message());
			return false;
		}

		// Validate the secret identifier
		if (memcmp(mReceiveBuffer, RemoteComSecret, sizeof(RemoteComSecret)) != 0)
		{
			LOG_ERROR("Invalid message format: Secret identifier mismatch");
			return false;
		}

		size_t		 offset			 = sizeof(RemoteComSecret);

		// Extract message type
		const size_t messageTypeSize = sizeof(message.type);
		memcpy(&message.type, &mReceiveBuffer[offset], messageTypeSize);
		offset += messageTypeSize;

		// Extract message data size
		size_t		 dataLength			   = 0;
		const size_t messageDataSizeLength = sizeof(message.data.size());
		memcpy(&dataLength, &mReceiveBuffer[offset], messageDataSizeLength);
		offset += messageDataSizeLength;


		// Second phase: Read the actual data

		if (dataLength <= 0)
		{
			message.data.clear();
			return false;
		}

		// Check for buffer overflow
		if (dataLength > PackageBufferSize)
		{
			LOG_ERROR("Message data size ({} bytes) exceeds buffer capacity!", dataLength);
			return false;
		}

		bytesRead = boost::asio::read(mSocket, boost::asio::buffer(mReceiveBuffer, dataLength), boost::asio::transfer_exactly(dataLength), ec);

		if (ec)
		{
			LOG_ERROR("Error reading message data: {}", ec.message());
			return false;
		}

		// Copy data to message
		message.data.assign(mReceiveBuffer, mReceiveBuffer + dataLength);

		return true;
	}
	catch (const std::exception &e)
	{
		LOG_ERROR("Exception while reading message: {}", e.what());
		return false;
	}
}
