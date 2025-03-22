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
}


tcp::socket &TCPSession::socket()
{
	return mSocket;
}


void TCPSession::start()
{
	readHeader();
}


void TCPSession::readHeader()
{
	auto self = shared_from_this();

	boost::asio::async_read(mSocket, boost::asio::buffer(mHeader, 4),
							[this, self](boost::system::error_code ec, size_t length)
							{
								if (ec)
								{
									return;
								}
								uint32_t netLength;
								std::memcpy(&netLength, mHeader, 4);
								mBodyLength = ntohl(netLength);

								mBody.resize(mBodyLength);
								readBody();
							});
}


void TCPSession::readBody()
{
	auto self = shared_from_this();
	boost::asio::async_read(mSocket, boost::asio::buffer(mBody),
							[this, self](boost::system::error_code ec, size_t length)
							{
								if (ec)
								{
									return;
								}

								if (mBodyLength < 4) // We expect at least 4 bytes for the type
								{
									LOG_ERROR("Body length < 4 bytes. Protocol error?");
									return;
								}

								// Extract JSON string
								std::string jsonData(mBody.data(), mBodyLength);

								// parse JSON
								try
								{
									json j = json::parse(jsonData);
									receivedMessage(j);
								}
								catch (std::exception &e)
								{
									LOG_ERROR("JSON parse error: {}", e.what());
								}

								readHeader(); // Read next message
							});
}


void TCPSession::sendJson(MultiplayerMessageType type, const json &message)
{
	std::string			 body			  = message.dump(); // convert j to string

	// The body is: [4 bytes of type] + [N bytes of JSON data]
	// We'll compute "bodyLength" = 4 + jsonData.size().
	// Then in the 4-byte TCP header, we store bodyLength in network order.
	uint32_t			 typeNetworkOrder = htonl(static_cast<uint32_t>(type));
	uint32_t			 bodyLen		  = 4 + static_cast<uint32_t>(body.size());
	uint32_t			 bodyLenNetwork	  = htonl(bodyLen);


	// Prepare a buffer of size (4 [header] + bodyLen)
	// The first 4 bytes => "header" (which is bodyLen)
	// The next 4 bytes => "type"
	// The remaining bytes => JSON data
	std::vector<uint8_t> buffer(4 + bodyLen);
	std::memcpy(buffer.data(), &bodyLenNetwork, 4);			  // Copy 4 byte header
	std::memcpy(buffer.data() + 4, &typeNetworkOrder, 4);	  // Copy 4 byte message type
	std::memcpy(buffer.data() + 8, body.data(), body.size()); // Copy the JSON string


	// Write it async
	auto self = shared_from_this();
	boost::asio::async_write(mSocket, boost::asio::buffer(buffer),
							 [this, self](boost::system::error_code ec, std::size_t /*bytesWritten*/)
							 {
								 if (ec)
								 {
									 LOG_ERROR("Error writing message: {}", ec.message());
								 }
							 });
}


void TCPSession::attachObserver(std::weak_ptr<IRemoteCommunicationObserver> observer)
{
	mObservers.push_back(observer);
}


void TCPSession::detachObserver(std::weak_ptr<IRemoteCommunicationObserver> observer)
{
	// Remove observer from the vector by checking if they point to the same object
	mObservers.erase(std::remove_if(mObservers.begin(), mObservers.end(),
									[&observer](const std::weak_ptr<IRemoteCommunicationObserver> &obs)
									{
										// Compare the objects they point to, not the weak_ptrs themselves
										if (obs.expired() || observer.expired())
										{
											return false; // Can't compare expired weak_ptrs
										}
										return !obs.owner_before(observer) && !observer.owner_before(obs);
										// This is equivalent to obs.lock() == observer.lock() without the overhead
									}),
					 mObservers.end());
}


void TCPSession::receivedMessage(const json &j)
{
	for (auto &observer : mObservers)
	{
		auto obs = observer.lock();

		if (obs)
			obs->onMessageReceived(j);
	}
}
