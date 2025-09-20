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


class RemoteSender : public IRemoteSenderObservable, public IMoveObserver, public IConnectionStatusObserver
{
public:
	RemoteSender()	= default;
	~RemoteSender() = default;

	void sendMessage(MultiplayerMessageType type, std::vector<uint8_t> &message) override;

	void onExecuteMove(const PossibleMove &move, bool fromRemote = false) override;
	void onAddToMoveHistory(Move &move) override {}
	void onClearMoveHistory() override {}

	void sendConnectionResponse(const InvitationResponse &response);
	void sendConnectionInvite(const InvitationRequest &invite);

	void onConnectionStateChanged(const ConnectionStatusEvent event) override;
	void onLocalPlayerChosen(const PlayerColor localPlayer) override;
	void onRemotePlayerChosen(const PlayerColor remote) override {}
	void onLocalReadyFlagSet(const bool flag) override;

private:
	std::vector<uint8_t> convertDataToByteVector(json &source);

	void				 sendMove(const PossibleMove &move);

	void				 sendConnectionState(const ConnectionState &state);

	void				 sendChatMessage(std::string &message);
};
