/*
  ==============================================================================

	Class:          Bishop

	Description:    Bishop class inheriting from ChessPiece

  ==============================================================================
*/


#include "Bishop.h"
#include "ChessBoard.h"


int Bishop::getPieceValue() const
{
	return bishopValue;
}


std::vector<PossibleMove> Bishop::getPossibleMoves(const Position &pos, ChessBoard &board) const
{
	MoveHelper helper;
	PieceColor color = getColor();
	helper.checkAvailableMoves(pos, board, color, PieceType::Bishop);
	auto moves = helper.getAvailableMoves();
	return moves;
}
