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

	mSendThread.reset(new SendThread(this));
	mReceiveThread.reset(new ReceiveThread(this));

	mIsInitialized.store(true);
	return true;
}


void RemoteCommunication::deinit()
{
	if (mTCPSession)
	{
		mTCPSession->stopReadAsync();
		mTCPSession.reset();
		mTCPSession = nullptr;
	}

	stop();

	mIsInitialized.store(false);
}


void RemoteCommunication::start()
{
	if (!isInitialized())
		return;

	// Start async read
	mTCPSession->startReadAsync(
		[this](MultiplayerMessageStruct message)
		{
			// Queue the message
			{
				std::lock_guard<std::mutex> lock(mIncomingListMutex);
				mIncomingMessages.push_back(message);
			};
		});

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
	// Get all messages from the queue
	std::vector<MultiplayerMessageStruct> messages;

	{
		std::lock_guard<std::mutex> lock(mIncomingListMutex);
		if (mIncomingMessages.empty())
			return true; // No messages, but not an error

		messages.swap(mIncomingMessages);
	}

	// Process all received messages
	for (auto &message : messages)
	{
		receivedMessage(message.type, message.data);
	}

	return true;
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
