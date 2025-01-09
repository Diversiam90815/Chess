/*
  ==============================================================================

	Class:          Pawn

	Description:    Pawn class inheriting from ChessPiece

  ==============================================================================
*/


#include "Pawn.h"
#include "ChessBoard.h"



std::vector<PossibleMove> Pawn::getPossibleMoves(const Position &pos, ChessBoard &board, bool attackOnly) const
{
	MoveHelper helper;
	bool	   moved = hasMoved();
	PlayerColor color	= getColor();
	helper.checkAvailableMoves(pos, board, color, PieceType::Pawn, moved, attackOnly);
	auto moves = helper.getAvailableMoves();
	return moves;
}