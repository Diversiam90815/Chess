/*
  ==============================================================================

	Class:          LoggingHelper

	Description:    Helper class for simplifying logging messages

  ==============================================================================
*/

#include "LoggingHelper.h"
#include "Logging.h"


std::string LoggingHelper::playerColourToString(PlayerColor player)
{
	switch (player)
	{
	case PlayerColor::NoColor: return "No Color";
	case PlayerColor::White: return "White";
	case PlayerColor::Black: return "Black";
	default: return "Unknown PlayerColor";
	}
}


std::string LoggingHelper::positionToString(Position pos)
{
	return "(x=" + std::to_string(pos.x) + ", y=" + std::to_string(pos.y) + ")";
}


std::string LoggingHelper::pieceTypeToString(PieceType piece)
{
	switch (piece)
	{
	case PieceType::DefaultType: return "Default Type";
	case PieceType::Pawn: return "Pawn";
	case PieceType::Knight: return "Knight";
	case PieceType::Bishop: return "Bishop";
	case PieceType::Rook: return "Rook";
	case PieceType::Queen: return "Queen";
	case PieceType::King: return "King";
	default: return "Unknown PieceType";
	}
}


std::string LoggingHelper::gameStateToString(GameState state)
{
	switch (state)
	{
	case GameState::Init: return "Init";
	case GameState::OnGoing: return "On Going";
	case GameState::Paused: return "Paused";
	case GameState::Checkmate: return "Checkmate";
	case GameState::Stalemate: return "Stalemate";
	case GameState::Draw: return "Draw";
	default: return "Unknown GameState";
	}
}


std::string LoggingHelper::moveStateToString(MoveState state)
{
	switch (state)
	{
	case MoveState::NoMove: return "No Move";
	case MoveState::InitiateMove: return "Initiate Move";
	case MoveState::ExecuteMove: return "Execute Move";
	default: return "Unknown MoveState";
	}
}


std::string LoggingHelper::moveTypeToString(MoveType type)
{
	if (type == MoveType::None)
		return "None";

	std::vector<std::string> types;

	// Helper lambda to check if a flag is set
	auto					 hasFlag = [&](MoveType flag) -> bool { return static_cast<int>(type) & static_cast<int>(flag); };

	if (hasFlag(MoveType::Normal))
		types.emplace_back("Normal");
	if (hasFlag(MoveType::DoublePawnPush))
		types.emplace_back("Double Pawn Push");
	if (hasFlag(MoveType::PawnPromotion))
		types.emplace_back("Pawn Promotion");
	if (hasFlag(MoveType::Capture))
		types.emplace_back("Capture");
	if (hasFlag(MoveType::EnPassant))
		types.emplace_back("En Passant");
	if (hasFlag(MoveType::CastlingKingside))
		types.emplace_back("Castling Kingside");
	if (hasFlag(MoveType::CastlingQueenside))
		types.emplace_back("Castling Queenside");
	if (hasFlag(MoveType::Check))
		types.emplace_back("Check");
	if (hasFlag(MoveType::Checkmate))
		types.emplace_back("Checkmate");

	std::string result;
	for (size_t i = 0; i < types.size(); ++i)
	{
		result += types[i];
		if (i != types.size() - 1)
			result += " | ";
	}

	return result;
}


void LoggingHelper::logMove(Move &move)
{
	LOG_INFO("----- Move Details -----");

	LOG_INFO("Move Number: %d", move.number);
	LOG_INFO("Player: %s", playerColourToString(move.player).c_str());

	LOG_INFO("Starting Position:");
	LOG_INFO("\n%s", positionToString(move.startingPosition).c_str());

	LOG_INFO("Ending Position:");
	LOG_INFO("\n%s", positionToString(move.endingPosition).c_str());

	LOG_INFO("Moved Piece: %s", pieceTypeToString(move.movedPiece).c_str());

	if (move.capturedPiece != PieceType::DefaultType)
	{
		LOG_INFO("Captured Piece: %s", pieceTypeToString(move.capturedPiece).c_str());
	}
	else
	{
		LOG_INFO("Captured Piece: None");
	}

	if (move.promotionType != PieceType::DefaultType)
	{
		LOG_INFO("Promotion Type: %s", pieceTypeToString(move.promotionType).c_str());
	}
	else
	{
		LOG_INFO("Promotion Type: None");
	}

	LOG_INFO("Move Type: %s", moveTypeToString(move.type).c_str());
	LOG_INFO("Notation: %s", move.notation.c_str());
	LOG_INFO("Half Move Clock: %d", move.halfMoveClock);

	LOG_INFO("------------------------\n");
}
