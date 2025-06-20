/*
  ==============================================================================
	Module:         RemoteCommunication
	Description:    Managing the messages themselves, sending and receiving
  ==============================================================================
*/

#include "RemoteCommunication.h"


bool RemoteCommunication::init(std::shared_ptr<TCPSession> session)
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
			// Validate and process the received message body
			if (message.data.size() < sizeof(RemoteComSecret))
			{
				LOG_ERROR("Received message is too small to contain secret identifier.");
				return;
			}

			if (memcmp(message.data.data(), RemoteComSecret, sizeof(RemoteComSecret)) != 0)
			{
				LOG_ERROR("Invalid message format: Secret identifier mismatch in body.");
				return;
			}

			// The secret is valid. Strip it from the data.
			message.data.erase(message.data.begin(), message.data.begin() + sizeof(RemoteComSecret));

			// Queue the message
			{
				std::lock_guard<std::mutex> lock(mIncomingListMutex);
				mIncomingMessages.push_back(message);
			};

			mReceiveThread->triggerEvent();
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
