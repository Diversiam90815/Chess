/*
  ==============================================================================
	Module:         RemoteReceiver
	Description:    Bridge between received messages and local chess session
  ==============================================================================
*/

#pragma once
#include "IObserver.h"
#include "IObservable.h"
#include "JsonConversion.h"
#include "Logging.h"


class RemoteReceiver : public IRemoteReceiverObserver, public IRemoteMessagesObservable
{
public:
	RemoteReceiver()  = default;
	~RemoteReceiver() = default;

	void onMessageReceived(MultiplayerMessageType type, std::vector<uint8_t> &message) override;

	void remoteConnectionStateReceived(const ConnectionState &state) override;

	void remoteMoveReceived(const PossibleMove &move) override;

	void remoteChatMessageReceived(const std::string &message) override;

private:
	template <typename T>
	T tryGetContentFromMessage(json message, std::string contentMessage);
};
