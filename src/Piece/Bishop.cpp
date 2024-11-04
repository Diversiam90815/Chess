/*
  ==============================================================================

	Class:          Bishop

	Description:    Bishop class inheriting from ChessPiece

  ==============================================================================
*/


#include "Bishop.h"
#include "ChessBoard.h"


std::vector<PossibleMove> Bishop::getPossibleMoves(const Position &pos, ChessBoard &board) const
{
	MoveHelper helper;
	PlayerColor color = getColor();
	helper.checkAvailableMoves(pos, board, color, PieceType::Bishop);
	auto moves = helper.getAvailableMoves();
	return moves;
}
