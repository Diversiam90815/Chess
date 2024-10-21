/*
  ==============================================================================

	Class:          King

	Description:    King class inheriting from ChessPiece

  ==============================================================================
*/


#pragma once

#include "ChessPiece.h"


class King : public ChessPiece
{
public:
	King(PieceColor color) : ChessPiece(PieceType::King, color)
	{
	}

	int								 getPieceValue() const override;

	std::vector<std::pair<int, int>> getPossibleMoves(int x, int y, ChessBoard &board) const override;

};
