/*
  ==============================================================================
	Module:         MessageDispatcher
	Description:    Delegating received messages in multiplayer mode
  ==============================================================================
*/

#include "MessageDispatcher.h"


MessageDispatcher::MessageDispatcher() {}


MessageDispatcher::~MessageDispatcher() {}


void MessageDispatcher::registerHandler(MultiplayerMessageType type, MultiplayerMessageHandler handler)
{
	mHandlers[type] = handler;
}


void MessageDispatcher::dispatchMessage(const json &j)
{
	if (!j.contains("msgType"))
		return;

	MultiplayerMessageType				 type	 = static_cast<MultiplayerMessageType>(j["msgType"].get<int>());
	std::unique_ptr<IMultiplayerMessage> message = createMessage(type, j);

	if (message && mHandlers.find(type) != mHandlers.end())
	{
		mHandlers[type](*message);
	}
}


std::unique_ptr<IMultiplayerMessage> MessageDispatcher::createMessage(MultiplayerMessageType type, const json &j)
{
	switch (type)
	{
	case MultiplayerMessageType::Move: return MoveMessage::fromJson(j);
	case MultiplayerMessageType::Chat: return ChatMessage::fromJson(j);
	default: return nullptr;
	}
}
