/*
  ==============================================================================
	Module:         RemoteSender
	Description:    Bridge between local chess session and outgoing messages
  ==============================================================================
*/

#include "RemoteSender.h"


void RemoteSender::sendMessage(MultiplayerMessageType type, std::vector<uint8_t> &message)
{
	for (auto &observer : mObservers)
	{
		if (auto obs = observer.lock())
			obs->onSendMessage(type, message);
	}
}


void RemoteSender::onExecuteMove(const PossibleMove &move, bool fromRemote)
{
	if (fromRemote)
		return; // This move came from the remote, so we do not need to send it

	sendMove(move);
}


void				 RemoteSender::onConnectionStateChanged(const ConnectionStatusEvent event) {}


std::vector<uint8_t> RemoteSender::convertDataToByteVector(json &source)
{
	std::vector<uint8_t> data;

	if (source.size() == 0)
	{
		data.clear();
		return data;
	}

	std::string sData = source.dump();
	data.assign(sData.begin(), sData.end());
	return data;
}


void RemoteSender::sendMove(const PossibleMove &move)
{
	json j;
	j[MoveKey] = move;

	auto data  = convertDataToByteVector(j);

	sendMessage(MultiplayerMessageType::Move, data);
}


void RemoteSender::sendConnectionState(const ConnectionState &state)
{
	json j;
	j[ConnectionStateKey] = state;

	auto data	= convertDataToByteVector(j);

	sendMessage(MultiplayerMessageType::ConnectionState, data);
}


void RemoteSender::sendChatMessage(std::string &message)
{
	json j;
	j[ChatMessageKey] = message;

	auto data		  = convertDataToByteVector(j);

	sendMessage(MultiplayerMessageType::Chat, data);
}
