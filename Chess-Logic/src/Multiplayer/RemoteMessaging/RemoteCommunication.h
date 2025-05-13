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


class RemoteCommunication : public IRemoteReceiverObservable, public IRemoteSenderObserver
{
public:
	RemoteCommunication()  = default;
	~RemoteCommunication() = default;

	bool init(std::shared_ptr<TCPSession> session);
	void deinit();

	void start();
	void stop();

	void onSendMessage(MultiplayerMessageType type, std::vector<uint8_t> &message) override;

	bool read(MultiplayerMessageType &type, std::vector<uint8_t> &dest);
	void write(MultiplayerMessageType type, std::vector<uint8_t> data);

	bool isInitialized() const { return mIsInitialized.load(); }

	bool receiveMessages();
	bool sendMessages();

private:
	void								  notifyObservers();
	void								  receivedMessage(MultiplayerMessageType type, std::vector<uint8_t> &message) override;

	std::atomic<bool>					  mIsInitialized{false};

	std::shared_ptr<TCPSession>			  mTCPSession;

	std::shared_ptr<SendThread>			  mSendThread;
	std::shared_ptr<ReceiveThread>		  mReceiveThread;

	std::mutex							  mIncomingListMutex;
	std::mutex							  mOutgoingListMutex;

	std::vector<MultiplayerMessageStruct> mIncomingMessages;
	std::vector<MultiplayerMessageStruct> mOutgoingMessages;
};
