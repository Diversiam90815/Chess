/*
  ==============================================================================
	Module:         MessageManager
	Description:    Managing received messages in multiplayer mode
  ==============================================================================
*/

#pragma once

#include <json.hpp>

#include "MessageDispatcher.h"
#include "MoveMessage.h"
#include "GameManager.h"

using json = nlohmann::json;


class MessageManager
{
public:
	MessageManager();
	~MessageManager();

	void onMessageReceived(const json &j); // TBI with observer


private:
	MessageDispatcher mDispatcher;

	void			  handleMoveMessage(const IMultiplayerMessage& message);
};