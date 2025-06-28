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
	Default			= 0,
	ConnectionState = 1,
	Move			= 2,
	Chat			= 3
};


struct MultiplayerMessageStruct
{
	MultiplayerMessageType type = MultiplayerMessageType::Default;
	std::vector<uint8_t>   data;
};
