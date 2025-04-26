/*
  ==============================================================================
	Module:         RemoteCommunication
	Description:    Managing the messages themselves, sending and receiving
  ==============================================================================
*/

#include "RemoteCommunication.h"


bool RemoteCommunication::init(boost::shared_ptr<TCPSession> session)
{
	mTCPSession = session;
	mIsInitialized.store(true);
	return true;
}


void RemoteCommunication::deinit()
{
	mTCPSession.reset();
	mTCPSession = nullptr;
	mIsInitialized.store(false);
	mIsInitialized.store(false);
}


void RemoteCommunication::start()
{
	mSendThread	   = boost::thread(&RemoteCommunication::runSendThread, this);
	mReceiveThread = boost::thread(&RemoteCommunication::runReceiveThread, this);
}


void RemoteCommunication::stop() {}


void RemoteCommunication::runSendThread()
{
	while (mSendThreadRunning.load())
	{
		/*
		mSenderCV.wait_for()
		sendMessages();*/

	}
}


void RemoteCommunication::runReceiveThread() {}


bool RemoteCommunication::read(MultiplayerMessageType &type, std::vector<uint8_t> &dest)
{
	std::lock_guard<std::mutex> lock(mIncomingListMutex);

	for (auto &message : mIncomingMessages)
	{
		type = message.type;
		dest = message.data;
		return true;
	}
	return false;
}


void RemoteCommunication::write(MultiplayerMessageType type, std::vector<uint8_t> data)
{
	std::lock_guard<std::mutex> lock(mOutgoingListMutex);

	MultiplayerMessageStruct	message;
	message.type = type;
	message.data = data;

	mOutgoingMessages.push_back(message);
}


void RemoteCommunication::notifyObservers()
{
	MultiplayerMessageType type;
	std::vector<uint8_t>   data;

	while (read(type, data))
	{
		receivedMessage(type, data);
	}
}


void RemoteCommunication::receivedMessage(MultiplayerMessageType type, std::vector<uint8_t> &message)
{
	for (auto &observer : mObservers)
	{
		if (auto obs = observer.lock())
			obs->onMessageReceived(type, message);
	}
}


bool RemoteCommunication::receiveMessages()
{
	MultiplayerMessageStruct message;
	bool					 success = false;

	while (true)
	{
		message.data.clear();
		message.type = MultiplayerMessageType::Default;

		success		 = mTCPSession->readMessage(message);

		if (!success)
			return false;

		{
			std::lock_guard<std::mutex> lock(mIncomingListMutex);

			mIncomingMessages.push_back(message);
			continue;
		}
	}
}


bool RemoteCommunication::sendMessages()
{
	for (auto &message : mOutgoingMessages)
	{
		bool success = mTCPSession->sendMessage(message);

		if (!success)
			return false;
	}
	return true;
}
