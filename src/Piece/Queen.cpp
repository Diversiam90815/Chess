/*
  ==============================================================================

	Class:          Queen

	Description:    Queen class inheriting from ChessPiece

  ==============================================================================
*/


#include "Queen.h"
#include "ChessBoard.h"


int Queen::getPieceValue() const
{
	return queenValue;
}


std::vector<Move> Queen::getPossibleMoves(const Position &pos, ChessBoard &board) const
{
	MoveHelper helper;
	PieceColor color = getColor();
	helper.checkAvailableMoves(pos, board, color, PieceType::Queen);
	auto moves = helper.getAvailableMoves();
	return moves;
}
