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
	LOG_INFO("----- Move Number {} -----", move.number);

	LOG_INFO("\tPlayer:\t\t{}", playerColourToString(move.player).c_str());

	LOG_INFO("\tStart:\t\t{}", positionToString(move.startingPosition).c_str());
	LOG_INFO("\tEnd:\t\t{}", positionToString(move.endingPosition).c_str());

	LOG_INFO("\tMoved Piece:\t\t{}", pieceTypeToString(move.movedPiece).c_str());

	if (move.capturedPiece != PieceType::DefaultType)
	{
		LOG_INFO("\tCaptured Piece:\t\t{}", pieceTypeToString(move.capturedPiece).c_str());
	}
	else
	{
		LOG_INFO("\tCaptured Piece:\t\tNone");
	}

	if (move.promotionType != PieceType::DefaultType)
	{
		LOG_INFO("\tPromotion Type:\t\t{}", pieceTypeToString(move.promotionType).c_str());
	}
	else
	{
		LOG_INFO("\tPromotion Type:\t\tNone");
	}

	LOG_INFO("\tMove Type:\t\t{}", moveTypeToString(move.type).c_str());
	LOG_INFO("\tNotation:\t\t{}", move.notation.c_str());
	LOG_INFO("\tHalf Move Clock:\t{}", move.halfMoveClock);

	LOG_INFO("------------------------");
}
