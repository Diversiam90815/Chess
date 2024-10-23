/*
  ==============================================================================

	Class:          Bishop

	Description:    Bishop class inheriting from ChessPiece

  ==============================================================================
*/


#pragma once

#include "ChessPiece.h"
#include "Parameters.h"
#include "MoveHelper.h"


class Bishop : public ChessPiece
{
public:
	Bishop(PieceColor color) : ChessPiece(PieceType::Bishop, color)
	{
	}


	int								 getPieceValue() const override;

	std::vector<PossibleMove>		 getPossibleMoves(const Position &pos, ChessBoard &board) const override;
};
