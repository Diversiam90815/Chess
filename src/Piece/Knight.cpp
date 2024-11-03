/*
  ==============================================================================

	Class:          Knight

	Description:    Knight class inheriting from ChessPiece

  ==============================================================================
*/


#include "Knight.h"
#include "ChessBoard.h"


std::vector<PossibleMove> Knight::getPossibleMoves(const Position &pos, ChessBoard &board) const
{
	MoveHelper helper;
	PieceColor color = getColor();
	helper.checkAvailableMoves(pos, board, color, PieceType::Knight);
	auto moves = helper.getAvailableMoves();
	return moves;
}