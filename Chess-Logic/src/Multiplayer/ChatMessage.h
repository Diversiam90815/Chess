/*
  ==============================================================================
	Module:         ChatMessage
	Description:    Message type containing chat information
  ==============================================================================
*/

#pragma once

#include <json.hpp>

#include "IMultiplayerMessage.h"

using json = nlohmann::json;


class ChatMessage : IMultiplayerMessage
{
public:
	MultiplayerMessageType getMessageType() const override { return MultiplayerMessageType::Chat; }

	json				   toJson() const override
	{
		json j;
		j["msgType"] = static_cast<int>(getMessageType());
		j["message"] = message;
		return j;
	}


	static std::unique_ptr<ChatMessage> fromJson(const json &j)
	{
		auto msg	 = std::make_unique<ChatMessage>();
		msg->message = j.at("message").get<std::string>();
		return msg;
	}


	std::string			   message;
};
