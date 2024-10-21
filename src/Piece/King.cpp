/*
  ==============================================================================

	Class:          King

	Description:    King class inheriting from ChessPiece

  ==============================================================================
*/


#include "King.h"
#include "ChessBoard.h"


int King::getPieceValue() const
{
	return kingValue;
}


std::vector<Move> King::getPossibleMoves(const Position &pos, ChessBoard &board) const
{
	MoveHelper helper;
	PieceColor color = getColor();
	helper.checkAvailableMoves(pos, board, color, PieceType::King);
	auto moves = helper.getAvailableMoves();
	return moves;
}
