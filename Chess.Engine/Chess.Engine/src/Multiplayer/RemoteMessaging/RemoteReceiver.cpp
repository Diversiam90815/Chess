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

	case MultiplayerMessageType::ConnectionState:
	{
		ConnectionStatusEvent state = tryGetContentFromMessage<ConnectionStatusEvent>(jMessage, ConnectionStateKey);

		remoteConnectionStateReceived(state.state);

		LOG_INFO("Received a connection state from remote: {}", LoggingHelper::connectionStateToString(state.state).c_str());
		break;
	}

	case MultiplayerMessageType::Move:
	{
		PossibleMove remoteMove = tryGetContentFromMessage<PossibleMove>(jMessage, MoveKey);

		if (remoteMove.isEmpty())
		{
			LOG_ERROR("Remote move is empty after decoding! There has been an error!");
			return;
		}

		LOG_INFO("Received move from remote:");
		LOG_INFO("\tStart: {0},{1}", remoteMove.start.x, remoteMove.start.y);
		LOG_INFO("\tEnd: {0},{1}", remoteMove.end.x, remoteMove.end.y);
		LOG_INFO("\tType: {0}", LoggingHelper::moveTypeToString(remoteMove.type));

		remoteMoveReceived(remoteMove);
		break;
	}

	case MultiplayerMessageType::Chat:
	{
		std::string chatMessage = tryGetContentFromMessage<std::string>(jMessage, ChatMessageKey);

		if (chatMessage.empty())
		{
			LOG_WARNING("Decoding chat message failed! Message appears to be empty!");
			return;
		}

		LOG_INFO("Received Chat message");

		remoteChatMessageReceived(chatMessage);
		break;
	}

	case MultiplayerMessageType::InvitationRequest:
	{
		InvitationRequest invite = tryGetContentFromMessage<InvitationRequest>(jMessage, InvitationMessageKey);
		LOG_INFO("Received invitation request from {}", invite.playerName);
		remoteInvitationReceived(invite);

		break;
	}

	case MultiplayerMessageType::InvitationResponse:
	{
		InvitationResponse invResponse = tryGetContentFromMessage<InvitationResponse>(jMessage, InvitationResponseMessageKey);
		LOG_INFO("Received invitation response from {}", invResponse.playerName);
		remoteInvitationResponseReceived(invResponse);

		break;
	}

	case MultiplayerMessageType::LocalPlayer:
	{
		PlayerColor remotePlayer = tryGetContentFromMessage<PlayerColor>(jMessage, PlayerChosenKey);

		LOG_INFO("Received player chosen from remote message Remote Player : {}", LoggingHelper::playerColourToString(remotePlayer));
		remotePlayerChosenReceived(remotePlayer);

		break;
	}

	case MultiplayerMessageType::PlayerReadyForGameFlag:
	{
		bool flag = tryGetContentFromMessage<bool>(jMessage, PlayerReadyFlagKey);

		LOG_INFO("Received player ready for game flag : {}", LoggingHelper::boolToString(flag));
		remotePlayerReadyFlagReceived(flag);

		break;
	}

	default:
	{
		int iType = static_cast<int>(type);
		LOG_WARNING("Could not decode message type. Received type: {}", iType);
		break;
	}
	}
}


void RemoteReceiver::remoteConnectionStateReceived(const ConnectionState &state)
{
	for (auto &observer : mObservers)
	{
		if (auto obs = observer.lock())
			obs->onRemoteConnectionStateReceived(state);
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


void RemoteReceiver::remoteInvitationReceived(const InvitationRequest &invite)
{
	for (auto &observer : mObservers)
	{
		if (auto obs = observer.lock())
			obs->onRemoteInvitationReceived(invite);
	}
}


void RemoteReceiver::remoteInvitationResponseReceived(const InvitationResponse &response)
{
	for (auto &observer : mObservers)
	{
		if (auto obs = observer.lock())
			obs->onRemoteInvitationResponseReceived(response);
	}
}


void RemoteReceiver::remotePlayerChosenReceived(const PlayerColor player)
{
	for (auto &observer : mObservers)
	{
		if (auto obs = observer.lock())
			obs->onRemotePlayerChosenReceived(player);
	}
}


void RemoteReceiver::remotePlayerReadyFlagReceived(const bool flag)
{
	for (auto &observer : mObservers)
	{
		if (auto obs = observer.lock())
			obs->onRemotePlayerReadyFlagReceived(flag);
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
