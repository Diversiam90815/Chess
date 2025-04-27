/*
  ==============================================================================
	Module:         RemoteSender
	Description:    Bridge between local chess session and outgoing messages
  ==============================================================================
*/

#pragma once

#include "IObserver.h"
#include "IObservable.h"
#include "Logging.h"


class RemoteSender : public IRemoteSenderObservable, public IMoveObserver
{
public:
	RemoteSender()	= default;
	~RemoteSender() = default;

	void sendMessage(MultiplayerMessageType type, std::vector<uint8_t> &message) override;

	void onExecuteMove(const PossibleMove &move) override;
	void onAddToMoveHistory(Move &move) override {}

private:
	std::vector<uint8_t> convertDataToByteVector(json &source);

	void				 sendMove(const PossibleMove &move);

	void				 sendChatMessage(std::string &message);
};
