/*
  ==============================================================================

	Class:          Queen

	Description:    Queen class inheriting from ChessPiece

  ==============================================================================
*/


#pragma once

#include "ChessPiece.h"
#include "Parameters.h"


class Queen : public ChessPiece
{
public:
	Queen(PieceColor color) : ChessPiece(PieceType::Queen, color)
	{
	}

	int								 getPieceValue() const override;

	std::vector<std::pair<int, int>> getPossibleMoves(int x, int y, ChessBoard &board) const override;
};
