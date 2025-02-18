/*
  ==============================================================================
	Module:         Pawn
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
	Pawn(PlayerColor color) : ChessPiece(PieceType::Pawn, color)
	{
	}


	std::vector<PossibleMove> getPossibleMoves(const Position &pos, ChessBoard &board, bool attackOnly = false) const override;
};
