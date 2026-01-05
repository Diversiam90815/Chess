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

//
// void RemoteSender::onExecuteMove(const PossibleMove &move, bool fromRemote)
//{
//	if (fromRemote)
//		return; // This move came from the remote, so we do not need to send it
//
//	sendMove(move);
//}


void RemoteSender::sendConnectionResponse(const InvitationResponse &response)
{
	json j;
	j[RemoteControl::InvitationResponseMessageKey] = response;

	auto data									   = convertDataToByteVector(j);

	sendMessage(MultiplayerMessageType::InvitationResponse, data);
}


void RemoteSender::sendConnectionInvite(const InvitationRequest &invite)
{
	json j;
	j[RemoteControl::InvitationMessageKey] = invite;

	auto data							   = convertDataToByteVector(j);

	sendMessage(MultiplayerMessageType::InvitationRequest, data);
}


void RemoteSender::onConnectionStateChanged(const ConnectionStatusEvent event)
{
	json j;
	j[RemoteControl::ConnectionStateKey] = event;

	auto data							 = convertDataToByteVector(j);

	sendMessage(MultiplayerMessageType::ConnectionState, data);
}


void RemoteSender::onLocalPlayerChosen(const Side localPlayer)
{
	json j;
	j[RemoteControl::PlayerChosenKey] = localPlayer;

	auto data						  = convertDataToByteVector(j);

	sendMessage(MultiplayerMessageType::LocalPlayer, data);
}


void RemoteSender::onLocalReadyFlagSet(const bool flag)
{
	json j;
	j[RemoteControl::PlayerReadyFlagKey] = flag;

	auto data							 = convertDataToByteVector(j);

	sendMessage(MultiplayerMessageType::PlayerReadyForGameFlag, data);
}


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


void RemoteSender::sendMove(const Move &move)
{
	json j;

	// // TODO: serialize move
	// j[RemoteControl::MoveKey] = move;

	auto data = convertDataToByteVector(j);

	sendMessage(MultiplayerMessageType::Move, data);
}


void RemoteSender::sendConnectionState(const ConnectionState &state)
{
	json j;
	j[RemoteControl::ConnectionStateKey] = state;

	auto data							 = convertDataToByteVector(j);

	sendMessage(MultiplayerMessageType::ConnectionState, data);
}


void RemoteSender::sendChatMessage(std::string &message)
{
	json j;
	j[RemoteControl::ChatMessageKey] = message;

	auto data						 = convertDataToByteVector(j);

	sendMessage(MultiplayerMessageType::Chat, data);
}
