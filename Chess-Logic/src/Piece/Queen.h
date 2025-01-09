/*
  ==============================================================================

	Class:          Queen

	Description:    Queen class inheriting from ChessPiece

  ==============================================================================
*/


#pragma once

#include "ChessPiece.h"
#include "Parameters.h"
#include "MoveHelper.h"


class Queen : public ChessPiece
{
public:
	Queen(PlayerColor color) : ChessPiece(PieceType::Queen, color)
	{
	}

	std::vector<PossibleMove> getPossibleMoves(const Position &pos, ChessBoard &board, bool attackOnly = false) const override;
};
