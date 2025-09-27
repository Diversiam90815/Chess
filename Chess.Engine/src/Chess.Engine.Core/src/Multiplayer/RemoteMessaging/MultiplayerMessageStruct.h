/*
  ==============================================================================
	Module:         MultiplayerMessageStruct
	Description:    Base struct defining the messages being sent/received
  ==============================================================================
*/

#pragma once

#include <string>
#include <vector>

/// <summary>
/// Defines the types of messages that can be exchanged in a multiplayer environment.
/// </summary>
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

/// <summary>
/// Represents a message used in multiplayer communication, containing a message type and associated data.
/// </summary>
struct MultiplayerMessageStruct
{
	MultiplayerMessageType type = MultiplayerMessageType::Default;
	std::vector<uint8_t>   data;
};

/// <summary>
/// Represents a request to invite a player, including the player's name and a version string for compatibility checks.
/// </summary>
struct InvitationRequest
{
	std::string playerName; // Player Name
	std::string version;	// For future compatibility checks

	bool		isValid() const { return !playerName.empty(); }
};

/// <summary>
/// Represents a response to an invitation, including acceptance status, player name, and an optional reason for declining.
/// </summary>
struct InvitationResponse
{
	bool		accepted;	// Was the request accpeted?
	std::string playerName; // Playername
	std::string reason;		// Specify reason if declined

	bool		isValid() const { return !playerName.empty(); }
};
