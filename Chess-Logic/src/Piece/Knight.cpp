/*
  ==============================================================================

	Class:          Knight

	Description:    Knight class inheriting from ChessPiece

  ==============================================================================
*/


#include "Knight.h"
#include "ChessBoard.h"


std::vector<PossibleMove> Knight::getPossibleMoves(const Position &pos, ChessBoard &board, bool attackOnly) const
{
	MoveHelper helper;
	PlayerColor color = getColor();
	helper.checkAvailableMoves(pos, board, color, PieceType::Knight, attackOnly);
	auto moves = helper.getAvailableMoves();
	return moves;
}