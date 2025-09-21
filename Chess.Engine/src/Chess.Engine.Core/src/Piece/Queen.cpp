/*
  ==============================================================================
	Module:         Queen
	Description:    Queen class inheriting from ChessPiece
  ==============================================================================
*/


#include "Queen.h"
#include "ChessBoard.h"


std::vector<PossibleMove> Queen::getPossibleMoves(const Position &pos, ChessBoard &board, bool attackOnly) const
{
	MoveHelper	helper;
	PlayerColor color = getColor();
	helper.checkAvailableMoves(pos, board, color, PieceType::Queen, attackOnly);
	auto moves = helper.getAvailableMoves();
	return moves;
}
