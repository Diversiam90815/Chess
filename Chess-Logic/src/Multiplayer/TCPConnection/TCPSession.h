/*
  ==============================================================================
	Module:         TCPSession
	Description:    Managing the socket and session used for the multiplayer mode
  ==============================================================================
*/

#pragma once

#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <functional>
#include <vector>
#include <json.hpp>

#include "Logging.h"
#include "IObservable.h"
#include "IMultiplayerMessage.h"

/*
	The message we're sending/receiving consists of 2 sections: header and body.
	Header will be 4 bytes long and contain the length of the body
	Body will contain the type (first 4 bytes of body) of message and the data itself preceding
*/

//
// enum class TCPMessageType : uint32_t
//{
//	Move = 1,
//	test = 2
//	// more to come
//};


using json = nlohmann::json;
using boost::asio::ip::tcp;

using MessageHandler = std::function<void(MultiplayerMessageType type, const json &message)>;


class TCPSession : public boost::enable_shared_from_this<TCPSession>, public IRemoteCommunicationObservable
{
public:
	typedef boost::shared_ptr<TCPSession> pointer;

	static pointer						  create(boost::asio::io_context &io_context) { return pointer(new TCPSession(io_context)); }

	tcp::socket							 &socket();

	void								  start();

	const int							  getBoundPort() const { return mBoundPort; }

	void								  sendJson(MultiplayerMessageType type, const json &message);

	void								  receivedMessage(const json &j) override;

private:
	explicit TCPSession(boost::asio::io_context &ioContext);

	void													 readHeader();

	void													 readBody();


	tcp::socket												 mSocket;

	char													 mHeader[4]{};
	std::vector<char>										 mBody{};
	uint32_t												 mBodyLength{0};

	int														 mBoundPort{0};

	MessageHandler											 mMessageHandler = nullptr;
};
