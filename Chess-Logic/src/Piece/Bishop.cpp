/*
  ==============================================================================

	Class:          Bishop

	Description:    Bishop class inheriting from ChessPiece

  ==============================================================================
*/


#include "Bishop.h"
#include "ChessBoard.h"


std::vector<PossibleMove> Bishop::getPossibleMoves(const Position &pos, ChessBoard &board, bool attackOnly) const
{
	MoveHelper helper;
	PlayerColor color = getColor();
	helper.checkAvailableMoves(pos, board, color, PieceType::Bishop, attackOnly);
	auto moves = helper.getAvailableMoves();
	return moves;
}
