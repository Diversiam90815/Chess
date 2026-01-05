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


/**
 * @brief	High-level consumer of inbound multiplayer messages.
 *
 * Workflow:
 *  - RemoteCommunication delivers raw message type + payload bytes.
 *  - RemoteReceiver parses (JSON) and invokes strongly typed observer callbacks
 *    for upper layers (e.g., MultiplayerManager, StateMachine).
 *
 * Parsing Strategy:
 *  - Each message type dispatches to a dedicated handler that performs minimal
 *    validation before notifying observers.
 */
class RemoteReceiver : public IRemoteReceiverObserver, public IRemoteMessagesObservable
{
public:
	RemoteReceiver()  = default;
	~RemoteReceiver() = default;

	/**
	 * @brief	Entry point for raw inbound message.
	 * @param	type -> Message discriminant.
	 * @param	message -> Raw payload bytes.
	 */
	void onMessageReceived(MultiplayerMessageType type, std::vector<uint8_t> &message) override;

	void remoteConnectionStateReceived(const ConnectionState &state) override;
	void remoteMoveReceived(const Move &move) override;
	void remoteChatMessageReceived(const std::string &message) override;
	void remoteInvitationReceived(const InvitationRequest &invite) override;
	void remoteInvitationResponseReceived(const InvitationResponse &response) override;
	void remotePlayerChosenReceived(const Side player) override;
	void remotePlayerReadyFlagReceived(const bool flag) override;

private:
	/**
	 * @brief	Attempt to extract typed content from a JSON blob.
	 * @tparam	T -> Target type (must be constructible / deserializable).
	 * @param	message -> Parsed JSON object.
	 * @param	contentMessage -> Field name / context for diagnostics.
	 * @return	Deserialized object or default-constructed on failure.
	 */
	template <typename T>
	T tryGetContentFromMessage(json message, std::string contentMessage);
};
