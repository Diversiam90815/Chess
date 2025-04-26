/*
  ==============================================================================
	Module:         RemoteCommunication
	Description:    Managing the messages themselves, sending and receiving
  ==============================================================================
*/

#pragma once

#include <boost/thread.hpp>

#include "MultiplayerMessageStruct.h"
#include "TCPConnection/TCPSession.h"


class RemoteCommunication : public IRemoteReceiverObservable
{
public:
	RemoteCommunication()  = default;
	~RemoteCommunication() = default;

	bool init(boost::shared_ptr<TCPSession> session);
	void deinit();

	void start();
	void stop();

	void runSendThread();
	void runReceiveThread();

	bool read(MultiplayerMessageType &type, std::vector<uint8_t> &dest);
	void write(MultiplayerMessageType type, std::vector<uint8_t> data);

	bool isInitialized() const { return mIsInitialized.load(); }

private:
	void								  notifyObservers();
	void								  receivedMessage(MultiplayerMessageType type, std::vector<uint8_t> &message) override;

	bool								  receiveMessages();
	bool								  sendMessages();

	std::atomic<bool>					  mIsInitialized{false};

	boost::shared_ptr<TCPSession>		  mTCPSession;

	boost::thread						  mSendThread;
	boost::thread						  mReceiveThread;

	std::mutex							  mIncomingListMutex;
	std::mutex							  mOutgoingListMutex;
	std::mutex							  mSendThreadMutex;
	std::mutex							  mReceiveThreadMutex;

	std::condition_variable				  mSenderCV;
	std::condition_variable				  mReceiverCV;

	std::atomic<bool>					  mSendThreadRunning{false};
	std::atomic<bool>					  mReceiveThreadRunning{false};

	std::vector<MultiplayerMessageStruct> mIncomingMessages;
	std::vector<MultiplayerMessageStruct> mOutgoingMessages;
};
