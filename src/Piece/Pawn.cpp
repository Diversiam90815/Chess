/*
  ==============================================================================

	Class:          Pawn

	Description:    Pawn class inheriting from ChessPiece

  ==============================================================================
*/


#include "Pawn.h"
#include "ChessBoard.h"



std::vector<PossibleMove> Pawn::getPossibleMoves(const Position &pos, ChessBoard &board) const
{
	MoveHelper helper;
	bool	   hasMoved = getHasMoved();
	PieceColor color	= getColor();
	helper.checkAvailableMoves(pos, board, color, PieceType::Pawn, hasMoved);
	auto moves = helper.getAvailableMoves();
	return moves;
}