/*
  ==============================================================================
	Module:         King
	Description:    King class inheriting from ChessPiece
  ==============================================================================
*/

#pragma once

#include "ChessPiece.h"
#include "Parameters.h"
#include "MoveHelper.h"

/// <summary>
/// King class inheriting from ChessPiece
/// </summary>
class King : public ChessPiece
{
public:
	King(PlayerColor color) : ChessPiece(PieceType::King, color) {}

	std::vector<PossibleMove> getPossibleMoves(const Position &pos, ChessBoard &board, bool attackOnly = false) const override;
};
