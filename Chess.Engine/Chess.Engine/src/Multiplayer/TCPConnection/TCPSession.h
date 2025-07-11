/*
  ==============================================================================
	Module:         TCPSession
	Description:    Managing the socket and session used for the multiplayer mode
  ==============================================================================
*/

#pragma once

#include <asio.hpp>
#include <vector>

#include "TCPInterfaces.h"
#include "Logging.h"
#include "IObservable.h"
#include "RemoteMessaging/MultiPlayerMessageStruct.h"


using asio::ip::tcp;


struct AsyncReadState
{
	MultiplayerMessageType messageType;
	size_t				   dataLength = 0;
};


class TCPSession : public ITCPSession, public std::enable_shared_from_this<TCPSession>
{
public:
	~TCPSession();

	typedef std::shared_ptr<TCPSession> pointer;

	static pointer						create(asio::io_context &io_context) { return pointer(new TCPSession(io_context)); }

	static pointer						create(tcp::socket socket) { return pointer(new TCPSession(std::move(socket))); }

	tcp::socket						   &socket();

	int									getBoundPort() const override { return mBoundPort; }

	bool								isConnected() const override;

	bool								sendMessage(MultiplayerMessageStruct &message) override;

	void								startReadAsync(MessageReceivedCallback callback) override;
	void								stopReadAsync() override;


private:
	explicit TCPSession(asio::io_context &ioContext);
	explicit TCPSession(tcp::socket &&socket);

	void					readMessageAsync();


	tcp::socket				mSocket;

	uint8_t				   *mReceiveBuffer = nullptr;
	uint8_t				   *mSendBuffer	   = nullptr;
	AsyncReadState			mReadState;

	MessageReceivedCallback mMessageReceivedCallback;

	bool					mAsyncReadActive{false};

	int						mBoundPort{0};
};
