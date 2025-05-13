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
	Default				= 0,
	ConnectionRequested = 1,
	ConnectionAccepted	= 2,
	ConnectionRejected	= 3,
	Move				= 4,
	Chat				= 5
};


struct MultiplayerMessageStruct
{
	MultiplayerMessageType type = MultiplayerMessageType::Default;
	std::vector<uint8_t>   data;
};
