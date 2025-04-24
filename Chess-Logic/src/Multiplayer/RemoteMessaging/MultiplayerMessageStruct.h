/*
  ==============================================================================
	Module:         MultiplayerMessageStruct
	Description:    Base struct defining the messages being sent/received
  ==============================================================================
*/

#pragma once

#include <string>
#include <vector>


enum class MessageType : uint32_t
{
	Move = 1,
	Chat = 2
	// To be expanded
};


struct MultiplayerMessageStruct
{
	std::vector<uint8_t> data;

	std::string			 remoteIPv4;
	int					 remotePort;

	MessageType			 messageType;
	uint64_t			 messageID;
};
