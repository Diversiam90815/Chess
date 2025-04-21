/*
  ==============================================================================
	Module:         MoveMessage
	Description:    Message type containing move information
  ==============================================================================
*/

#pragma once

#include <memory>

#include "IMultiplayerMessage.h"
#include "Move.h"
#include "MoveSerialization.h"

using json = nlohmann::json;


class MoveMessage : public IMultiplayerMessage
{
public:
	MoveMessage() = default;
	MoveMessage(PossibleMove &possibleMove) : move(possibleMove) {};

	MultiplayerMessageType getMessageType() const override { return MultiplayerMessageType::Move; }


	json				   toJson() const override
	{
		json j;
		j["msgType"] = static_cast<int>(getMessageType());
		j["move"]	 = move;
		return j;
	}


	static std::unique_ptr<MoveMessage> fromJson(const json &j)
	{
		auto msg  = std::make_unique<MoveMessage>();
		msg->move = j.at("move").get<PossibleMove>();
		return msg;
	}

private:
	PossibleMove move;
};