/*
  ==============================================================================

	Class:          Knight

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

	Knight(PieceColor color) : ChessPiece(PieceType::Knight, color)
	{
	}

	int								 getPieceValue() const override;

	std::vector<PossibleMove>		 getPossibleMoves(const Position &pos, ChessBoard &board) const override;
	
};
