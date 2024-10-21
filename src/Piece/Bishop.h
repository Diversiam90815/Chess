/*
  ==============================================================================

	Class:          Bishop

	Description:    Bishop class inheriting from ChessPiece

  ==============================================================================
*/


#pragma once

#include "ChessPiece.h"
#include "Parameters.h"


class Bishop : public ChessPiece
{
public:
	Bishop(PieceColor color) : ChessPiece(PieceType::Bishop, color)
	{
	}


	int								 getPieceValue() const override;

	std::vector<std::pair<int, int>> getPossibleMoves(int x, int y, ChessBoard &board) const override;
};
