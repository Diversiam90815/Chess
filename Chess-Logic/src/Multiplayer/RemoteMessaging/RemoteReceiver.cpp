/*
  ==============================================================================
	Module:         RemoteReceiver
	Description:    Bridge between received messages and local chess session
  ==============================================================================
*/

#include "RemoteReceiver.h"


void RemoteReceiver::onMessageReceived(MultiplayerMessageType type, std::vector<uint8_t> &message)
{
	json jMessage;

	if (message.size() != 0)
	{
		jMessage = json::parse(message.begin(), message.end(), nullptr, false);

		if (jMessage.is_discarded())
		{
			std::string messageData = std::string(message.begin(), message.end());
			LOG_WARNING("Received broken message!");
			LOG_WARNING("Message could not be converted to JSON : {}", messageData.c_str());
			return;
		}
	}

	switch (type)
	{
	case MultiplayerMessageType::Default:
	{
		LOG_WARNING("Caught a non initialized message package! Skipping this one!");
		break;
	}

	case MultiplayerMessageType::Move:
	{
		PossibleMove remoteMove = tryGetContentFromMessage<PossibleMove>(jMessage, "Move");

		if (remoteMove.isEmpty())
		{
			LOG_ERROR("Remote move is empty after decoding! There has been an error!");
			return;
		}

		remoteMoveReceived(remoteMove);
		break;
	}

	case MultiplayerMessageType::Chat:
	{
		std::string chatMessage = tryGetContentFromMessage<std::string>(jMessage, "Message");

		if (chatMessage.empty())
		{
			LOG_WARNING("Decoding chat message failed! Message appears to be empty!");
			return;
		}

		remoteChatMessageReceived(chatMessage);
		break;
	}
	default: break;
	}
}


void RemoteReceiver::remoteMoveReceived(const PossibleMove &move)
{
	for (auto &observer : mObservers)
	{
		if (auto obs = observer.lock())
			obs->onRemoteMoveReceived(move);
	}
}


void RemoteReceiver::remoteChatMessageReceived(const std::string &message)
{
	for (auto &observer : mObservers)
	{
		if (auto obs = observer.lock())
			obs->onRemoteChatMessageReceived(message);
	}
}


template <typename T>
inline T RemoteReceiver::tryGetContentFromMessage(json message, std::string contentMessage)
{
	T content = T();

	try
	{
		content = message[contentMessage];
	}
	catch (const json::type_error &e)
	{
		return content;
	}

	return content;
}
