/*
  ==============================================================================
	Module:         TCPSession
	Description:    Managing the socket and session used for the multiplayer mode
  ==============================================================================
*/

#pragma once

#include <boost/asio.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <functional>
#include <vector>

#include "Logging.h"
#include "IObservable.h"
#include "RemoteMessaging/MultiPlayerMessageStruct.h"


using boost::asio::ip::tcp;


class TCPSession : public boost::enable_shared_from_this<TCPSession>
{
public:
	~TCPSession();

	typedef boost::shared_ptr<TCPSession> pointer;

	static pointer						  create(boost::asio::io_context &io_context) { return pointer(new TCPSession(io_context)); }

	tcp::socket							 &socket();

	const int							  getBoundPort() const { return mBoundPort; }

	bool								  isConnected() const;

	bool								  sendMessage(MultiplayerMessageStruct &message);

	bool								  readMessage(MultiplayerMessageStruct &message);

private:
	explicit TCPSession(boost::asio::io_context &ioContext);


	tcp::socket	  mSocket;

	uint8_t		 *mReceiveBuffer = nullptr;
	uint8_t		 *mSendBuffer = nullptr;

	int			  mBoundPort{0};

};
