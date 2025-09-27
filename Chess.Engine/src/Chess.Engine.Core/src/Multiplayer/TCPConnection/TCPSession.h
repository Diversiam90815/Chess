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

/**
 * @brief Represents the state of an asynchronous read operation currently in progress.
 */
struct AsyncReadState
{
	MultiplayerMessageType messageType;
	size_t				   dataLength = 0;
};


/**
 * @brief Concrete TCP session implementing message-based async read/write abstraction.
 *
 * Life-cycle:
 *  - Created via static create() helpers (shared_ptr managed).
 *  - startReadAsync() begins message framing loop; stopReadAsync() cancels it.
 *  - sendMessage() serializes and dispatches a message buffer.
 *
 * Thread-safety: Public methods intended to be invoked from owning I/O thread;
 * minimal external synchronization assumed.
 */
class TCPSession : public ITCPSession, public std::enable_shared_from_this<TCPSession>
{
public:
	~TCPSession();

	typedef std::shared_ptr<TCPSession> pointer;

	/**
	 * @brief Factory: allocate session owning its own socket (not yet connected).
	 */
	static pointer						create(asio::io_context &io_context) { return pointer(new TCPSession(io_context)); }

	/**
	 * @brief Factory: wrap an already accepted or connected socket.
	 */
	static pointer						create(tcp::socket socket) { return pointer(new TCPSession(std::move(socket))); }

	/**
	 * @brief Access underlying socket (for low-level operations).
	 */
	tcp::socket						   &socket();

	int									getBoundPort() const override { return mBoundPort; }
	bool								isConnected() const override;

	/**
	 * @brief Serialize and send a multiplayer message (may queue internally).
	 * @return true if dispatch initiated.
	 */
	bool								sendMessage(MultiplayerMessageStruct &message) override;

	/**
	 * @brief Start async read loop delivering complete messages to callback.
	 */
	void								startReadAsync(MessageReceivedCallback callback) override;

	/**
	 * @brief Stop current async read loop (safe if not active).
	 */
	void								stopReadAsync() override;


private:
	explicit TCPSession(asio::io_context &ioContext);
	explicit TCPSession(tcp::socket &&socket);

	/**
	 * @brief Internal continuation-based async read dispatcher handling headers and payload.
	 */
	void					readMessageAsync();


	tcp::socket				mSocket;

	uint8_t				   *mReceiveBuffer = nullptr;
	uint8_t				   *mSendBuffer	   = nullptr;
	AsyncReadState			mReadState;

	MessageReceivedCallback mMessageReceivedCallback;

	bool					mAsyncReadActive{false};

	int						mBoundPort{0};
};
