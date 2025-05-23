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


void TCPSession::readHeaderAsync()
{
	if (!mAsyncReadActive || !isConnected())
		return;

	// Calculate Header size
	const size_t headerSize = sizeof(RemoteComSecret) + sizeof(MultiplayerMessageType) + sizeof(size_t);

	// Start async read for message header
	asio::async_read(mSocket, asio::buffer(mReceiveBuffer, headerSize), asio::transfer_exactly(headerSize),
					 [this](const asio::error_code &ec, size_t bytesTransfered)
					 {
						 if (!mAsyncReadActive)
							 return;

						 if (ec)
						 {
							 LOG_ERROR("Error reading message header : {}", ec.message().c_str());

							 // If connection is still active, try reading again
							 if (isConnected() && mAsyncReadActive)
								 readHeaderAsync();

							 return;
						 }

						 // Process the header
						 processHeader(bytesTransfered);
					 });
}


void TCPSession::readMessageBodyAsync(size_t dataLength, MultiplayerMessageType messageType)
{
	if (!mAsyncReadActive || !isConnected())
		return;

	asio::async_read(mSocket, asio::buffer(mReceiveBuffer, dataLength), asio::transfer_exactly(dataLength),
					 [this, dataLength, messageType](const asio::error_code &ec, size_t bytesTransfered)
					 {
						 if (!mAsyncReadActive)
							 return;

						 if (ec)
						 {
							 LOG_ERROR("Error reading message body: {}", ec.message().c_str());

							 // If still active, read next message
							 if (mAsyncReadActive && isConnected())
								 readHeaderAsync();

							 return;
						 }

						 // Create complete message
						 MultiplayerMessageStruct message;
						 message.type = messageType;
						 message.data.assign(mReceiveBuffer, mReceiveBuffer + dataLength);

						 // Deliver message
						 if (mMessageReceivedCallback)
							 mMessageReceivedCallback(message);

						 // Continue reading the next message
						 readHeaderAsync();
					 });
}


void TCPSession::processHeader(size_t bytesTransfered)
{
	// Validate the secret identifier
	if (memcmp(mReceiveBuffer, RemoteComSecret, sizeof(RemoteComSecret)) != 0)
	{
		LOG_ERROR("Invalid message format: Secret identifier mismatch");
		readHeaderAsync(); // Continue listening for next message
		return;
	}

	size_t		 offset			 = sizeof(RemoteComSecret);

	// Extract message type
	const size_t messageTypeSize = sizeof(MultiplayerMessageType);
	memcpy(&mReadState.messageType, &mReceiveBuffer[offset], messageTypeSize);
	offset += messageTypeSize;

	// Extract message data size
	memcpy(&mReadState.dataLength, &mReceiveBuffer[offset], sizeof(size_t));

	// Check for buffer overflow
	if (mReadState.dataLength > PackageBufferSize)
	{
		LOG_ERROR("Message data size ({} bytes) exceeds buffer capacity!", mReadState.dataLength);
		readHeaderAsync(); // read next message
		return;
	}

	// If we have data to read, proceed with reading the message body
	if (mReadState.dataLength > 0)
	{
		readMessageBodyAsync(mReadState.dataLength, mReadState.messageType);
	}
	else
	{
		// empty message: create message with just the type and no data
		MultiplayerMessageStruct message;
		message.type = mReadState.messageType;
		message.data.clear();

		// Deliver message
		if (mMessageReceivedCallback)
			mMessageReceivedCallback(message);
	}

	// Continue reading next message
	readHeaderAsync();
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
	readHeaderAsync();
}


void TCPSession::stopReadAsync()
{
	mAsyncReadActive = false;
}
