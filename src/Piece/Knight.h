/*
  ==============================================================================

	Class:          Knight

	Description:    Knight class inheriting from ChessPiece

  ==============================================================================
*/


#pragma once

#include "ChessPiece.h"
#include "Parameters.h"


class Knight : public ChessPiece
{
public:

	Knight(PieceColor color) : ChessPiece(PieceType::Knight, color)
	{
	}

	int								 getPieceValue() const override;

	std::vector<std::pair<int, int>> getPossibleMoves(int x, int y, ChessBoard &board) const override;
	
};
