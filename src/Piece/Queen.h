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
	Queen(PieceColor color) : ChessPiece(PieceType::Queen, color)
	{
	}

	std::vector<PossibleMove> getPossibleMoves(const Position &pos, ChessBoard &board) const override;
};
