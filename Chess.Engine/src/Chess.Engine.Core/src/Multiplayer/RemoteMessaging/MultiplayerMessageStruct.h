/*
  ==============================================================================
	Module:         MultiplayerMessageStruct
	Description:    Base struct defining the messages being sent/received
  ==============================================================================
*/

#pragma once

#include <string>
#include <vector>

/**
 * @file	MultiplayerMessageStruct.h
 * @brief	Core multiplayer messaging primitives.
 *
 * Serialization model:
 *  - All messages are sent as a small header (message type enum value) followed by
 *    an opaque byte vector ('std::vector<uint8_t>').
 *  - Higher level systems (RemoteSender / RemoteReceiver) JSON‑serialize
 *    domain objects into the 'data' buffer but the container
 *    itself is agnostic.
 *
 * Threading:
 *  - These structs are POD-like and intentionally lightweight; they are copied
 *    between producer (sender thread) and consumer (receiver thread) queues.
 *
 * Validation:
 *  - Lightweight 'isValid()' helpers allow early rejection before deeper parsing.
 */


/**
 * @brief	Discriminant for the semantic meaning / payload contract of a message.
 *
 * Value meanings:
 *  - Default:					Uninitialized / placeholder (should not normally be transmitted).
 *  - ConnectionState:			Broadcast or directed update of current connection state machine.
 *  - Move:						Encodes a single chess move (origin, destination, promotion, etc.).
 *  - Chat:						Plain text chat message between peers.
 *  - InvitationRequest:		Client -> Host connection attempt (includes player name / version).
 *  - InvitationResponse:		Host -> Client accept / reject decision.
 *  - PlayerReadyForGameFlag:	Ready toggle used to synchronize actual game start.
 *  - LocalPlayer:				Selection of local player's color communicated to the peer.
 */
enum class MultiplayerMessageType : uint32_t
{
	Default				   = 0,
	ConnectionState		   = 1,
	Move				   = 2,
	Chat				   = 3,
	InvitationRequest	   = 4,
	InvitationResponse	   = 5,
	PlayerReadyForGameFlag = 6,
	LocalPlayer			   = 7,
};


/**
 * @brief	Wire envelope: type discriminator + raw (already serialized) payload bytes.
 *
 * Life-cycle:
 *  - Filled by sender just before enqueue.
 *  - Transport layer (ITCPSession) writes raw buffer.
 *  - Receiver reconstructs the struct and routes by 'type'.
 *
 * Invariants:
 *  - 'type' must not remain Default when placed on an outgoing queue.
 *  - 'data' may be empty for messages whose semantic payload is entirely implicit
 *    (e.g. a ready flag toggle encoded as a single boolean, depending on protocol design).
 */
struct MultiplayerMessageStruct
{
	MultiplayerMessageType type = MultiplayerMessageType::Default;
	std::vector<uint8_t>   data;
};


/**
 * @brief	Client -> Host initial connection negotiation payload.
 *
 * Fields:
 *  - playerName:	Display name advertised to host (required, non-empty).
 *  - version:		Application version string (future negotiation).
 *
 * Validation:
 *  - 'isValid()' rejects empty player names.
 */
struct InvitationRequest
{
	std::string playerName;
	std::string version;

	bool		isValid() const { return !playerName.empty(); }
};


/**
 * @brief	Host -> Client response to earlier invitation.
 *
 * Fields:
 *  - accepted:		True if client request approved.
 *  - playerName:	Host (or reassigned) name context.
 *  - reason:		Optional human-readable rejection cause (only meaningful if !accepted).
 *
 * Validation:
 *  - 'isValid()' ensures a non-empty player name (aligning with request contract).
 */
struct InvitationResponse
{
	bool		accepted;
	std::string playerName;
	std::string reason;

	bool		isValid() const { return !playerName.empty(); }
};
