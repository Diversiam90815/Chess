/*
  ==============================================================================
	Module:         RemoteCommunication
	Description:    Managing the messages themselves, sending and receiving
  ==============================================================================
*/

#pragma once

#include "MultiplayerMessageStruct.h"
#include "TCPConnection/TCPSession.h"
#include "CommunicationThreads.h"


/**
 * @brief	Bidirectional messaging hub wrapping an established ITCPSession.
 *
 * Core Functions:
 *  - Owns send & receive worker threads (decouples I/O from game logic).
 *  - Maintains thread-safe incoming/outgoing message queues.
 *  - Bridges RemoteSender (produces outgoing) and RemoteReceiver (consumes incoming).
 *
 * Threading Model:
 *  - Public enqueue / read methods are thread-safe.
 *  - SendThread: waits for outgoing data event -> writes via ITCPSession.
 *  - ReceiveThread: monitors socket (async callback or polling) -> enqueues -> notifies observers.
 *
 * Lifecycle:
 *  1. init(session)     : binds ITCPSession (must be connected).
 *  2. start()           : starts worker threads.
 *  3. stop()/deinit()   : stops threads, clears queues, releases session.
 *
 * Failure Handling:
 *  - If session disconnects, send/receive loops naturally stop processing (caller
 *    can observe via higher level connection state mechanics).
 */
class RemoteCommunication : public IRemoteReceiverObservable, public IRemoteSenderObserver
{
public:
	RemoteCommunication()  = default;
	~RemoteCommunication() = default;

	/**
	 * @brief	Initialize communication with an established TCP session.
	 * @param	session -> Connected session (shared ownership).
	 * @return	true if initialization succeeded (non-null session).
	 * @note	Safe to call only once before start().
	 */
	bool init(std::shared_ptr<ITCPSession> session);

	/**
	 * @brief	Gracefully stop threads (if running) and release resources.
	 *			Clears pending messages.
	 */
	void deinit();

	/**
	 * @brief	Launch send/receive worker threads (idempotent).
	 * @pre		init() succeeded.
	 */
	void start();

	/**
	 * @brief	Stop worker threads; does not implicitly call deinit().
	 */
	void stop();

	/**
	 * @brief	Observer callback from RemoteSender: enqueue outgoing message.
	 * @param	type -> Message type discriminator.
	 * @param	message -> Serialized payload bytes.
	 */
	void onSendMessage(MultiplayerMessageType type, std::vector<uint8_t> &message) override;

	/**
	 * @brief	Attempt (blocking or buffered) read of a single message from session.
	 * @param	[OUT] type -> parsed message type.
	 * @param	[OUT] dest -> raw payload bytes.
	 * @return	false if no message or session not connected.
	 */
	bool read(MultiplayerMessageType &type, std::vector<uint8_t> &dest);

	/**
	 * @brief	Enqueue already serialized message for asynchronous transmission.
	 * @param	type -> Message type.
	 * @param	data -> Payload copy (moved into queue).
	 */
	void write(MultiplayerMessageType type, std::vector<uint8_t> data);

	bool isInitialized() const { return mIsInitialized.load(); }

	/**
	 * @brief	Drain incoming queue, dispatch each to registered IRemoteReceiverObserver.
	 * @return	true if at least one message processed.
	 */
	bool receiveMessages();

	/**
	 * @brief	Flush queued outgoing messages to session.
	 * @return	true if at least one message sent.
	 */
	bool sendMessages();

private:
	/**
	 * @brief	Internal dispatch of a single inbound message to observers.
	 */
	void								  receivedMessage(MultiplayerMessageType type, std::vector<uint8_t> &message) override;

	/**
	 * @brief	Clear both incoming and outgoing queues (used on teardown).
	 */
	void								  clearPendingMessages();


	std::atomic<bool>					  mIsInitialized{false};

	std::shared_ptr<ITCPSession>		  mTCPSession;

	std::shared_ptr<SendThread>			  mSendThread;
	std::shared_ptr<ReceiveThread>		  mReceiveThread;

	std::mutex							  mIncomingListMutex;
	std::mutex							  mOutgoingListMutex;

	std::vector<MultiplayerMessageStruct> mIncomingMessages;
	std::vector<MultiplayerMessageStruct> mOutgoingMessages;
};
