/*
  ==============================================================================
	Module:         RemoteCommunication
	Description:    Managing the messages themselves, sending and receiving
  ==============================================================================
*/

#include "RemoteCommunication.h"


bool RemoteCommunication::init(boost::shared_ptr<TCPSession> session)
{
	mTCPSession	   = session;

	mSendThread	   = std::make_shared<SendThread>(this);
	mReceiveThread = std::make_shared<ReceiveThread>(this);

	mIsInitialized.store(true);
	return true;
}


void RemoteCommunication::deinit()
{
	mTCPSession.reset();
	mTCPSession = nullptr;
	mIsInitialized.store(false);
}


void RemoteCommunication::start()
{
	mSendThread->start();
	mReceiveThread->start();
}


void RemoteCommunication::stop()
{
	mSendThread->stop();
	mReceiveThread->stop();
}


void RemoteCommunication::onSendMessage(MultiplayerMessageType type, std::vector<uint8_t> &message)
{
	if (!isInitialized())
	{
		LOG_WARNING("Could not sent message, since the Remote Communication is not initialized! Please initialize the Remote Communication before attempting to send messages!");
		return;
	}

	write(type, message);
}


bool RemoteCommunication::read(MultiplayerMessageType &type, std::vector<uint8_t> &dest)
{
	std::lock_guard<std::mutex> lock(mIncomingListMutex);

	if (mIncomingMessages.empty())
		return false;

	// Get the first message
	auto &message = mIncomingMessages.front();
	type		  = message.type;
	dest		  = message.data;

	// Remove the processed message
	mIncomingMessages.erase(mIncomingMessages.begin());
	return true;
}


void RemoteCommunication::write(MultiplayerMessageType type, std::vector<uint8_t> data)
{
	std::lock_guard<std::mutex> lock(mOutgoingListMutex);

	MultiplayerMessageStruct	message;
	message.type = type;
	message.data = data;

	mOutgoingMessages.push_back(message);
	mSendThread->triggerEvent();
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

		success		 = mTCPSession->readMessage(message);	// TODO: Make non blocking

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

	// Clear all messages after successful sending
	mOutgoingMessages.clear();
	return true;
}
