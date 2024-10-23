/*
  ==============================================================================

	Class:          Pawn

	Description:    Pawn class inheriting from ChessPiece

  ==============================================================================
*/


#pragma once

#include "ChessPiece.h"
#include "Parameters.h"
#include "MovementManager.h"


class Pawn : public ChessPiece
{
public:
	Pawn(PieceColor color) : ChessPiece(PieceType::Pawn, color)
	{
	}


	int				  getPieceValue() const override;

	std::vector<PossibleMove> getPossibleMoves(const Position &pos, ChessBoard &board) const override;
};
