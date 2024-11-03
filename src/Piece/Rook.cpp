/*
  ==============================================================================

	Class:          Rook

	Description:    Rook class inheriting from ChessPiece

  ==============================================================================
*/


#include "Rook.h"
#include "ChessBoard.h"


std::vector<PossibleMove> Rook::getPossibleMoves(const Position &pos, ChessBoard &board) const
{
	MoveHelper helper;
	PlayerColor color = getColor();
	helper.checkAvailableMoves(pos, board, color, PieceType::Rook);
	auto moves = helper.getAvailableMoves();
	return moves;
}
