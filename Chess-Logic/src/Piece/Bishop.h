/*
  ==============================================================================
	Module:         Bishop
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
	Bishop(PlayerColor color) : ChessPiece(PieceType::Bishop, color) {}


	std::vector<PossibleMove> getPossibleMoves(const Position &pos, ChessBoard &board, bool attackOnly = false) const override;
};
