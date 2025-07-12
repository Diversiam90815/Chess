/*
  ==============================================================================
	Module:         MultiplayerMessageStruct
	Description:    Base struct defining the messages being sent/received
  ==============================================================================
*/

#pragma once

#include <string>
#include <vector>


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


struct MultiplayerMessageStruct
{
	MultiplayerMessageType type = MultiplayerMessageType::Default;
	std::vector<uint8_t>   data;
};


struct InvitationRequest
{
	std::string playerName; // Player Name
	std::string version;	// For future compatibility checks

	bool		isValid() const { return !playerName.empty(); }
};

struct InvitationResponse
{
	bool		accepted;	// Was the request accpeted?
	std::string playerName; // Playername
	std::string reason;		// Specify reason if declined

	bool		isValid() const { return !playerName.empty(); }
};
