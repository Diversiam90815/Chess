/*
  ==============================================================================

	Class:          Pawn

	Description:    Pawn class inheriting from ChessPiece

  ==============================================================================
*/


#include "Pawn.h"
#include "ChessBoard.h"


int Pawn::getPieceValue() const
{
	return pawnValue;
}


std::vector<PossibleMove> Pawn::getPossibleMoves(const Position &pos, ChessBoard &board) const
{
	MoveHelper helper;
	bool	   hasMoved = getHasMoved();
	PieceColor color	= getColor();
	helper.checkAvailableMoves(pos, board, color, PieceType::Pawn, hasMoved);
	auto moves = helper.getAvailableMoves();
	return moves;
}