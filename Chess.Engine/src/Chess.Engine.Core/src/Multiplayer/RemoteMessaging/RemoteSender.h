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


/**
 * @brief	High-level producer of outgoing multiplayer messages.
 *
 * Responsibilities:
 *  - Translates domain events (moves, chat, invitations, ready flags, player choice)
 *    into serialized message payloads and emits them through its observer chain
 *    (ultimately handled by RemoteCommunication).
 *  - Observes local move execution (IMoveObserver) and connection state changes
 *    to broadcast them as needed.
 */
class RemoteSender : public IRemoteSenderObservable, public IConnectionStatusObserver
{
public:
	RemoteSender()	= default;
	~RemoteSender() = default;

	/**
	 * @brief	Emit an already prepared message (used internally after serialization).
	 * @param	type -> Message discriminator.
	 * @param	message -> Serialized payload bytes (may be modified/moved).
	 */
	void sendMessage(MultiplayerMessageType type, std::vector<uint8_t> &message) override;

	///**
	// * @brief	Observer: invoked when a move is executed locally.
	// * @param	move -> Executed move.
	// * @param	fromRemote -> True if the move originated remotely (avoid echo).
	// */
	// void onExecuteMove(const Move &move, bool fromRemote = false) override;

	// void onAddToMoveHistory(Move &move) override {}
	// void onClearMoveHistory() override {}

	/**
	 * @brief	Send host <-> client invitation response (accept / reject).
	 */
	void sendConnectionResponse(const InvitationResponse &response);

	/**
	 * @brief	Send client -> host invitation request.
	 */
	void sendConnectionInvite(const InvitationRequest &invite);

	/**
	 * @brief	Broadcast connection state changes (client or host).
	 */
	void onConnectionStateChanged(const ConnectionStatusEvent event) override;

	/**
	 * @brief	Notify remote of local player's chosen color.
	 */
	void onLocalPlayerChosen(const Side localPlayer) override;

	void onRemotePlayerChosen(const Side remote) override {}

	/**
	 * @brief	Communicate that the local player toggled ready flag.
	 */
	void onLocalReadyFlagSet(const bool flag) override;

private:
	/**
	 * @brief	Serialize a JSON object into raw bytes vector.
	 * @note	Returns empty vector if serialization fails.
	 */
	std::vector<uint8_t> convertDataToByteVector(json &source);

	/**
	 * @brief	Serialize and send a chess move.
	 */
	void				 sendMove(const Move &move);

	/**
	 * @brief	Serialize and broadcast connection state.
	 */
	void				 sendConnectionState(const ConnectionState &state);

	/**
	 * @brief	Serialize and send chat message.
	 */
	void				 sendChatMessage(std::string &message);
};
