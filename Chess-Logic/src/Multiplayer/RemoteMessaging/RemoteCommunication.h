/*
  ==============================================================================
	Module:         RemoteCommunication
	Description:    Managing the messages themselves, sending and receiving
  ==============================================================================
*/

#pragma once

#include <boost/thread.hpp>

#include "ThreadBase.h"
#include "MultiplayerMessageStruct.h"
#include "TCPConnection/TCPSession.h"


class SendThread : public ThreadBase
{
public:
	SendThread(RemoteCommunication *owner) : mOwner(owner) {}

protected:
	void run() override
	{
		waitForEvent(200);
		mOwner->sendMessages();
	}

private:
	RemoteCommunication *mOwner;
};


class ReceiveThread : public ThreadBase
{
public:
	ReceiveThread(RemoteCommunication *owner) : mOwner(owner) {}

protected:
	void run() override
	{
		waitForEvent(200);
		mOwner->receiveMessages();
	}

private:
	RemoteCommunication *mOwner;
};


class RemoteCommunication : public IRemoteReceiverObservable
{
public:
	RemoteCommunication()  = default;
	~RemoteCommunication() = default;

	bool init(boost::shared_ptr<TCPSession> session);
	void deinit();

	void start();
	void stop();

	bool read(MultiplayerMessageType &type, std::vector<uint8_t> &dest);
	void write(MultiplayerMessageType type, std::vector<uint8_t> data);

	bool isInitialized() const { return mIsInitialized.load(); }

	bool receiveMessages();
	bool sendMessages();

private:
	void								  notifyObservers();
	void								  receivedMessage(MultiplayerMessageType type, std::vector<uint8_t> &message) override;

	std::atomic<bool>					  mIsInitialized{false};

	boost::shared_ptr<TCPSession>		  mTCPSession;

	std::shared_ptr<SendThread>			  mSendThread;
	std::shared_ptr<ReceiveThread>		  mReceiveThread;

	std::mutex							  mIncomingListMutex;
	std::mutex							  mOutgoingListMutex;

	std::vector<MultiplayerMessageStruct> mIncomingMessages;
	std::vector<MultiplayerMessageStruct> mOutgoingMessages;
};
