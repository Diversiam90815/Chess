/*
  ==============================================================================
	Module:         MoveSerialization
	Description:    Convert structs for move handling to and from JSON
  ==============================================================================
*/

#pragma once

#include <json.hpp>
#include "Move.h"

using json = nlohmann::json;


inline void to_json(json &j, const PossibleMove &move)
{
	j = json{{"start", {{"x", move.start.x}, {"y", move.start.y}}},
			 {"end", {{"x", move.end.x}, {"y", move.end.y}}},
			 {"type", static_cast<int>(move.type)},
			 {"promotionPiece", static_cast<int>(move.promotionPiece)}};
}


inline void from_json(const json &j, PossibleMove &move)
{
	j.at("start").at("x").get_to(move.start.x);
	j.at("start").at("y").get_to(move.start.y);
	j.at("end").at("x").get_to(move.end.x);
	j.at("end").at("y").get_to(move.end.y);
	int typeInt;
	j.at("type").get_to(typeInt);
	move.type = static_cast<MoveType>(typeInt);
	int promo;
	j.at("promotionPiece").get_to(promo);
	move.promotionPiece = static_cast<PieceType>(promo);
}


inline void to_json(json &j, const Move &move)
{
	j = json{{"startingPosition", {{"x", move.startingPosition.x}, {"y", move.startingPosition.y}}},
			 {"endingPosition", {{"x", move.endingPosition.x}, {"y", move.endingPosition.y}}},
			 {"movedPiece", static_cast<int>(move.movedPiece)},
			 {"capturedPiece", static_cast<int>(move.capturedPiece)},
			 {"promotionType", static_cast<int>(move.promotionType)},
			 {"player", static_cast<int>(move.player)},
			 {"type", static_cast<int>(move.type)},
			 {"notation", move.notation},
			 {"number", move.number},
			 {"halfMoveClock", move.halfMoveClock}};
}


inline void from_json(const json &j, Move &move)
{
	j.at("startingPosition").at("x").get_to(move.startingPosition.x);
	j.at("startingPosition").at("y").get_to(move.startingPosition.y);
	j.at("endingPosition").at("x").get_to(move.endingPosition.x);
	j.at("endingPosition").at("y").get_to(move.endingPosition.y);
	int moved;
	j.at("movedPiece").get_to(moved);
	move.movedPiece = static_cast<PieceType>(moved);
	int captured;
	j.at("capturedPiece").get_to(captured);
	move.capturedPiece = static_cast<PieceType>(captured);
	int promo;
	j.at("promotionType").get_to(promo);
	move.promotionType = static_cast<PieceType>(promo);
	int player;
	j.at("player").get_to(player);
	move.player = static_cast<PlayerColor>(player);
	int typeInt;
	j.at("type").get_to(typeInt);
	move.type = static_cast<MoveType>(typeInt);
	j.at("notation").get_to(move.notation);
	j.at("number").get_to(move.number);
	j.at("halfMoveClock").get_to(move.halfMoveClock);
}
