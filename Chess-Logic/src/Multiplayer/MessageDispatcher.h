/*
  ==============================================================================
	Module:         MessageDispatcher
	Description:    Delegating received messages in multiplayer mode
  ==============================================================================
*/

#pragma once

#include <functional>
#include <unordered_map>
#include <json.hpp>
#include <memory>

#include "IMultiplayerMessage.h"
#include "MoveMessage.h"
#include "ChatMessage.h"


using json			 = nlohmann::json;
using MultiplayerMessageHandler  = std::function<void(const IMultiplayerMessage &)>;


class MessageDispatcher
{
public:
	MessageDispatcher();
	~MessageDispatcher();

	void registerHandler(MultiplayerMessageType type, MultiplayerMessageHandler handler);

	void dispatchMessage(const json &j);

private:
	std::unique_ptr<IMultiplayerMessage>					   createMessage(MultiplayerMessageType type, const json &j);

	std::unordered_map<MultiplayerMessageType, MultiplayerMessageHandler> mHandlers;
};