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
	MultiplayerMessageType getMessageType() const override { return MultiplayerMessageType::Move; }


	json		   toJson() const override
	{
		json j;
		j["type"]			 = static_cast<int>(getMessageType());
		j["start_x"]		 = move.start.x;
		j["start_y"]		 = move.start.y;
		j["end_x"]			 = move.end.x;
		j["end_y"]			 = move.end.y;
		j["move_type"]		 = static_cast<int>(move.type);
		j["promotion_piece"] = static_cast<int>(move.promotionPiece);
		return j;
	}

	static MoveMessage fromJson(const json &j)
	{
		MoveMessage msg;
		msg.move.start.x		= j["start_x"].get<int>();
		msg.move.start.y		= j["start_y"].get<int>();
		msg.move.end.x			= j["end_x"].get<int>();
		msg.move.end.y			= j["end_y"].get<int>();
		msg.move.type			= static_cast<MoveType>(j["move_type"].get<int>());
		msg.move.promotionPiece = static_cast<PieceType>(j["promotion_piece"].get<int>());
		return msg;
	}


private:
	PossibleMove move;
};