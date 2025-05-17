/*
  ==============================================================================
	Module:         Knight
	Description:    Knight class inheriting from ChessPiece
  ==============================================================================
*/

#pragma once

#include "ChessPiece.h"
#include "Parameters.h"
#include "MoveHelper.h"


class Knight : public ChessPiece
{
public:

	Knight(PlayerColor color) : ChessPiece(PieceType::Knight, color)
	{
	}

	std::vector<PossibleMove> getPossibleMoves(const Position &pos, ChessBoard &board, bool attackOnly = false) const override;
};
