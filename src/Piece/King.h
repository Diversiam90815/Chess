/*
  ==============================================================================

	Class:          King

	Description:    King class inheriting from ChessPiece

  ==============================================================================
*/


#pragma once

#include "ChessPiece.h"
#include "Parameters.h"
#include "MoveHelper.h"


class King : public ChessPiece
{
public:
	King(PieceColor color) : ChessPiece(PieceType::King, color)
	{
	}

	int								 getPieceValue() const override;

	std::vector<Move>				 getPossibleMoves(const Position &pos, ChessBoard &board) const override;

};
