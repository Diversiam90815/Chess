/*
  ==============================================================================
	Module:         IMultiplayerMessage
	Description:    Interface class for a message sent via TCP in Multiplayer mode
  ==============================================================================
*/

#pragma once
#include <json.hpp>

enum class MultiplayerMessageType : uint32_t
{
	Move = 1,
	Chat = 2
	// To be expanded
};


class IMultiplayerMessage
{
public:
	virtual ~IMultiplayerMessage() {};

	// Returns the type of the message
	virtual MultiplayerMessageType getMessageType() const = 0;

	// serialize message to JSON
	virtual nlohmann::json		   toJson() const		  = 0;
};
