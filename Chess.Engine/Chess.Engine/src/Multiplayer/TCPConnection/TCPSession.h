/*
  ==============================================================================
	Module:         TCPSession
	Description:    Managing the socket and session used for the multiplayer mode
  ==============================================================================
*/

#pragma once

#include <asio.hpp>
#include <functional>
#include <vector>

#include "Logging.h"
#include "IObservable.h"
#include "RemoteMessaging/MultiPlayerMessageStruct.h"


using asio::ip::tcp;


class TCPSession : public std::enable_shared_from_this<TCPSession>
{
public:
	~TCPSession();

	using MessageReceivedCallback = std::function<void(MultiplayerMessageStruct &message)>;

	typedef std::shared_ptr<TCPSession> pointer;

	static pointer						  create(asio::io_context &io_context) { return pointer(new TCPSession(io_context)); }

    static pointer create(tcp::socket socket) {  
        return pointer(new TCPSession(std::move(socket)));  
    }  

	tcp::socket							 &socket();

	const int							  getBoundPort() const { return mBoundPort; }

	bool								  isConnected() const;

	bool								  sendMessage(MultiplayerMessageStruct &message);

	void								  startReadAsync(MessageReceivedCallback callback);
	void								  stopReadAsync();

private:
	explicit TCPSession(asio::io_context &ioContext);
	explicit TCPSession(tcp::socket &&socket);

	void		readHeaderAsync();
	void		readMessageBodyAsync(size_t dataLength, MultiplayerMessageType messageType);

	void		processHeader();

	void		readFullMessageAsync();


	tcp::socket mSocket;

	uint8_t	   *mReceiveBuffer = nullptr;
	uint8_t	   *mSendBuffer	   = nullptr;

	struct AsyncReadState
	{
		MultiplayerMessageType messageType;
		size_t				   dataLength = 0;
	};

	AsyncReadState			mReadState;

	MessageReceivedCallback mMessageReceivedCallback;

	bool					mAsyncReadActive{false};

	int						mBoundPort{0};
};
