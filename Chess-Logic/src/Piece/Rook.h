/*
  ==============================================================================
	Module:         Rook
	Description:    Rook class inheriting from ChessPiece
  ==============================================================================
*/


#pragma once

#include "ChessPiece.h"
#include "Parameters.h"
#include "MoveHelper.h"


class Rook : public ChessPiece
{
public:
	Rook(PlayerColor color) : ChessPiece(PieceType::Rook, color)
	{
	}

	std::vector<PossibleMove> getPossibleMoves(const Position &pos, ChessBoard &board, bool attackOnly = false) const override
	{
		MoveHelper	helper;
		PlayerColor color = getColor();
		helper.checkAvailableMoves(pos, board, color, PieceType::Rook, attackOnly);
		auto moves = helper.getAvailableMoves();
		return moves;
	}
};
