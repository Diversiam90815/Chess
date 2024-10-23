/*
  ==============================================================================

	Class:          Rook

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
	Rook(PieceColor color) : ChessPiece(PieceType::Rook, color)
	{
	}

	int getPieceValue() const override;

	std::vector<PossibleMove> getPossibleMoves(const Position &pos, ChessBoard &board) const override;
};
