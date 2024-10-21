/*
  ==============================================================================

	Class:          Rook

	Description:    Rook class inheriting from ChessPiece

  ==============================================================================
*/


#include "Rook.h"
#include "ChessBoard.h"


int Rook::getPieceValue() const
{
	return rookValue;
}


std::vector<Move> Rook::getPossibleMoves(const Position &pos, ChessBoard &board) const
{
	MoveHelper helper;
	PieceColor color = getColor();
	helper.checkAvailableMoves(pos, board, color, PieceType::Rook);
	auto moves = helper.getAvailableMoves();
	return moves;
}
